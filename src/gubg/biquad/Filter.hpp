#ifndef HEADER_gubg_biquad_Filter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_biquad_Filter_hpp_ALREADY_INCLUDED

#include <gubg/biquad/Coefficients.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace biquad { 

    template <typename T>
    class Filter
    {
    public:
        using Coefficients = biquad::Coefficients<T>;

        bool set(const Coefficients &coeffs)
        {
            MSS_BEGIN(bool);
            MSS(coeffs.a0 > 0);
            b0_ = coeffs.b0/coeffs.a0;
            b1_ = coeffs.b1/coeffs.a0;
            b2_ = coeffs.b2/coeffs.a0;
            a1_ = -coeffs.a1/coeffs.a0;
            a2_ = -coeffs.a2/coeffs.a0;
            MSS_END();
        }

        T operator()(const T x0)
        {
            T y0 = 0;

            //y[n-0] = (b0*x[n-0] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2])/a0
            y0 += b0_*x0;
            y0 += b1_*x1_;
            y0 += b2_*x2_;
            y0 += a1_*y1_;
            y0 += a2_*y2_;

            x2_ = x1_;
            x1_ = x0;

            y2_ = y1_;
            y1_ = y0;

            return y0;
        }

    private:
        T b0_ = 0, b1_ = 0, b2_ = 0;
        T a0_ = 0, a1_ = 0, a2_ = 0;
        T x1_ = 0, x2_ = 0;
        T y1_ = 0, y2_ = 0;
    };

} } 

#endif
