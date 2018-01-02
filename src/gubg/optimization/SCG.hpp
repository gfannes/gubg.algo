#ifndef HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED

#include "gubg/debug.hpp"
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

        template <typename Function, typename Gradient>
        Float operator()(typename Params::Type &params, const Function &function, const Gradient &gradient)
        {
            S("");

            L("1. Initialization");
            {
                setup_(params);

                auto &h0 = history_data_[iteration_&1u];
                gradient(h0.r, h0.w);
                h0.p = h0.r;

                h0.eval = function(h0.w);
            }

            for (; true; ++iteration_)
            {
                auto &h0 = history_data_[iteration_&1u];
                auto &h1 = history_data_[(iteration_+1)&1u];

                outer_.scg_params(iteration_, h0.eval, h0.w);

                L("2. Calculate second order information");
                h0.ss_p = Params::sum_squares(h0.p);
                if (success_)
                {
                    //TODO: Causes division by zero sometimes
                    const Float sigma_k = sigma_/std::sqrt(h0.ss_p);
                    L(C(sigma_k));
                    h1.w = h0.w;
                    Params::update(h1.w, sigma_k, h0.p);
                    gradient(h1.r, h1.w);
                    Params::update(h1.r, -1.0, h0.r);
                    delta_k_ = -Params::inprod(h0.p, h1.r)/sigma_k;
                    L(C(delta_k_));
                }

                L("3. Scale");
                delta_k_ += (lambda_k_-lambda_bar_k_)*h0.ss_p;
                L(C(delta_k_));

                L("4. Make Hessian positive definite");
                if (delta_k_ <= 0)
                {
                    lambda_bar_k_ = 2.0*(lambda_k_-delta_k_/h0.ss_p);
                    delta_k_ = -delta_k_ + lambda_k_*h0.ss_p;
                    lambda_k_ = lambda_bar_k_;
                    L(C(delta_k_)C(lambda_k_)C(lambda_bar_k_));
                }
                assert(delta_k_ >= 0);

                L("5. Calculate step size");
                const Float mu_k = Params::inprod(h0.p, h0.r);
                const Float alpha_k = mu_k/delta_k_;
                L(C(mu_k)C(alpha_k));

                L("6. Comparison parameter");
                h1.w = h0.w;
                Params::update(h1.w, alpha_k, h0.p);
                h1.eval = function(h1.w);
                const Float diff_k = 2.0*delta_k_*(h1.eval - h0.eval)/(mu_k*mu_k);
                L(C(diff_k));

                L("7. Reduce error, if possible");
                if (diff_k >= 0.0)
                {
                    //h1.w is already correct from step 6
                    gradient(h1.r, h1.w);
                    lambda_bar_k_ = 0.0;
                    success_ = true;
                    if ((iteration_+1)%order_ == 0)
                    {
                        L("Restarting algorithm");
                        h1.p = h1.r;
                    }
                    else
                    {
                        L("Continuing ...");
                        const Float beta_k = (Params::sum_squares(h1.r)-Params::inprod(h1.r, h0.r))/mu_k;
                        h1.p = h1.r;
                        Params::update(h1.p, beta_k, h0.p);
                    }
                    if (diff_k >= 0.75)
                    {
                        L("Reducing the scale parameter");
                        lambda_k_ *= 0.25;
                    }
                }
                else
                {
                    lambda_bar_k_ = lambda_k_;
                    success_ = false;
                }

                L("8. Increase scale, if needed");
                if (diff_k < 0.25)
                {
                    L("Increasing the scale parameter");
                    lambda_k_ += delta_k_*(1.0-diff_k)/h0.ss_p;
                }

                L("9. Check for end");
                if (outer_.scg_terminate(iteration_, h0.eval, h0.r))
                {
                    L("We found a suitable maximum in iteration " << iteration_ << ": " << h0.eval);
                    params = h0.w;
                    return h0.eval;
                }
            }
        }

    private:
        void setup_(const typename Params::Type &params)
        {
            S("");

            order_ = Params::order(params);
            L(C(order_));

            iteration_ = 0;

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
        }

        unsigned int order_;
        unsigned int iteration_;
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
