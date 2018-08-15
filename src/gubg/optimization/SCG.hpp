#ifndef HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED

#include "gubg/debug.hpp"
#include "gubg/hr.hpp"
#include <array>
#include <cmath>
#include <cassert>

namespace gubg { namespace optimization { 

    namespace details { 
        struct DoNothing
        {
            template <typename Float ,typename Params>
            void scg_params(unsigned int iteration, Float eval, const Params &params) {}

            template <typename Float ,typename Params>
            bool scg_terminate(unsigned int iteration, Float eval, const Params &gradient)
            {
                for (auto g: gradient)
                    if (std::abs(g) > 0.000000001)
                        return false;
                return true;
            }
        };
    } 

    template <typename Float, typename Params, typename Outer = details::DoNothing>
    class SCG
    {
    public:
        SCG(): outer_(do_nothing_outer_) {}
        SCG(Outer &outer): outer_(outer) {}

        void clear()
        {
            do_setup_ = true;
            iteration_ = 0;
        }

        template <typename Function, typename Gradient>
        Float operator()(typename Params::Type &params, const Function &function, const Gradient &gradient)
        {
            S("");

            Float ret = 0.0;

            for (bool stop = false; !stop; ++iteration_)
            {
                auto &h0 = history_data_[iteration_&1u];
                auto &h1 = history_data_[(iteration_+1)&1u];

                if (setup_(params))
                {
                    L("1. Initialization");
                    L("     Doing full initialization");
                    gradient(h0.r, h0.w);
                    h0.p = h0.r;

                    h0.eval = function(h0.w);
                    L("     Current value: " << C(h0.eval));
                }

                L(C(iteration_)C(h0.eval)C(h1.eval));
                L("     " << C(hr(h0.w)));
                L("     " << C(hr(h0.r)));
                L("     " << C(hr(h0.p)));
                L("     " << C(hr(h1.r)));

                //Report current value and weights to outer_
                outer_.scg_params(iteration_, h0.eval, h0.w);

                L("2. Calculate second order information");
                h0.ss_p = Params::sum_squares(h0.p);
                L("     " << C(h0.ss_p));
                if (success_)
                {
                    L("     Success: updating parameters to increase output");
                    //TODO: Causes division by zero sometimes
                    const Float sigma_k = sigma_/std::sqrt(h0.ss_p);
                    L("     Epsilon used for approximating second order info " << C(sigma_k));
                    h1.w = h0.w;
                    Params::update(h1.w, sigma_k, h0.p);
                    gradient(h1.r, h1.w);
                    Params::update(h1.r, -1.0, h0.r);
                    delta_k_ = -Params::inprod(h0.p, h1.r)/sigma_k;
                    L("     Hessian positive definiteness along h0.p " << C(delta_k_));
                }

                L("3. Scale");
                delta_k_ += (lambda_k_-lambda_bar_k_)*h0.ss_p;
                L("     " << C(delta_k_));

                L("4. Make Hessian positive definite, if needed");
                if (delta_k_ <= 0)
                {
                    L("     Hessian is negative");
                    lambda_bar_k_ = 2.0*(lambda_k_-delta_k_/h0.ss_p);
                    delta_k_ = -delta_k_ + lambda_k_*h0.ss_p;
                    lambda_k_ = lambda_bar_k_;
                    L("     " << C(delta_k_)C(lambda_k_)C(lambda_bar_k_));
                }
                L("     " << C(lambda_k_));
                assert(delta_k_ >= 0);

                L("5. Calculate step size");
                const Float mu_k = Params::inprod(h0.p, h0.r);
                const Float alpha_k = mu_k/delta_k_;
                L("     " << C(mu_k)C(alpha_k));

                L("6. Comparison parameter");
                h1.w = h0.w;
                Params::update(h1.w, alpha_k, h0.p);
                h1.eval = function(h1.w);
                const Float diff_k = 2.0*delta_k_*(h1.eval - h0.eval)/(mu_k*mu_k);
                L("     New value: " << C(h1.eval)C(diff_k));

                L("7. Increase output, if possible");
                const bool could_increase = (diff_k >= 0.0);
                if (could_increase)
                {
                    L("     Could increase output to " << C(h1.eval));
                    //h1.w and h1.eval are already correct from step 6
                    gradient(h1.r, h1.w);
                    h1.ss_r = Params::sum_squares(h1.r);
                    if (h1.ss_r < 0.00001)
                    {
                        L("     STOP: gradient is becoming very small, we found a minimum " << C(h1.ss_r)C(h1.eval));
                        ret = h1.eval;
                        params = h1.w;
                        stop = true;
                    }
                    lambda_bar_k_ = 0.0;
                    success_ = true;
                    if ((iteration_+1)%order_ == 0)
                    {
                        L("     Restarting algorithm");
                        h1.p = h1.r;
                    }
                    else
                    {
                        L("     Continuing ...");
                        const Float beta_k = (h1.ss_r-Params::inprod(h1.r, h0.r))/mu_k;
                        h1.p = h1.r;
                        Params::update(h1.p, beta_k, h0.p);
                    }
                    if (diff_k >= 0.75)
                    {
                        lambda_k_ *= 0.25;
                        L("     Reducing the scale parameter to " << C(lambda_k_));
                    }
                }
                else
                {
                    L("     Could not increase output");
                    lambda_bar_k_ = lambda_k_;
                    h1 = h0;
                    success_ = false;
                }

                L("8. Increase scale, if needed");
                if (diff_k < 0.25)
                {
                    lambda_k_ += delta_k_*(1.0-diff_k)/h0.ss_p;
                    L("     Increasing the scale parameter to " << C(lambda_k_)C(h0.ss_p));
                }

                if (lambda_k_ > 1.0e100)
                {
                    L("Something strange is happening");
                    do_setup_ = true;
                }

                L("9. Check for end");
                const auto &h = (could_increase ? h1 : h0);
                if (outer_.scg_terminate(iteration_, h.eval, h.r))
                {
                    L("     STOP: user indicated we can stop " << iteration_ << ": " << C(h.eval));
                    ret = h.eval;
                    params = h.w;
                    stop = true;
                }
            }

            L(C(ret));
            return ret;
        }

    private:
        bool do_setup_ = true;
        bool setup_(const typename Params::Type &params)
        {
            if (!do_setup_)
                if (Params::order(params) == order_)
                    //Order is still the same: do not setup again
                    return false;

            S("");

            order_ = Params::order(params);
            L(C(order_));

            success_ = true;

            auto setup_history = [&](unsigned int t)
            {
                history_data_[t].w = params;
                history_data_[t].r = params;
                history_data_[t].p = params;
            };
            setup_history(0);
            setup_history(1);

            sigma_ = 0.001;
            lambda_k_ = 0.00001;
            lambda_bar_k_ = 0.0;
            L(C(sigma_)C(lambda_k_)C(lambda_bar_k_));

            do_setup_ = false;
            return true;
        }

        unsigned int order_;
        unsigned int iteration_ = 0;
        bool success_ = true;
        Float sigma_;

        struct Data
        {
            typename Params::Type w, r, p;
            Float ss_r, ss_p;
            Float eval;
        };
        Data history_data_[2];

        Float delta_k_;
        Float lambda_k_, lambda_bar_k_;

        details::DoNothing do_nothing_outer_;
        Outer &outer_;
    };

} } 

#endif
