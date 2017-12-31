#ifndef HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED

#include <cmath>

namespace gubg { namespace optimization { 

    namespace details { 
        struct Default
        {
        };
    } 

    template <typename Float, typename Params, typename Outer = details::Default>
    class SCG
    {
    public:
        SCG(): outer_(default_outer_) {}
        SCG(Outer &outer): outer_(outer) {}

        template <typename Function, typename Gradient>
        void operator()(const typename Params::Type &params, const Function &function, const Gradient &gradient)
        {
            //1. Choose initial parameters
            sigma_ = 0.001;

            w_ = params;
            r_ = params;

            gradient(r_, w_);
            p_ = r_;

            unsigned int k = 0;
            bool success = true;

            //2. Calculate second order information
            if (success)
            {
                sigma_k_ = sigma_/std::sqrt(Params::sum_squares(p_));
            }
        }

    private:
        Float sigma_;
        Float sigma_k_;
        typename Params::Type w_, p_, r_;
        typename Params::Type s_;

        details::Default default_outer_;
        Outer &outer_;
    };

} } 

#endif
