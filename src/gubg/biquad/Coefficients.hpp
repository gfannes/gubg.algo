#ifndef HEADER_gubg_biquad_Coefficients_hpp_ALREADY_INCLUDED
#define HEADER_gubg_biquad_Coefficients_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace biquad { 

    //a0*y[n-0] + a1*y[n-1] + a2*y[n-2] = b0*x[n-0] + b1*x[n-1] + b2*x[n-2]
    //=> y[n-0] = (b0*x[n-0] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2])/a0
    //=> H(z) = (b0 + b1*z^-1 + b2*z^-2)/(a0 + a1*z^-1 + a2*z^-2)

    template <typename T>
    struct Coefficients
    {
        using Self = Coefficients<T>;

        T b0 = 0;
        T b1 = 0;
        T b2 = 0;
        T a0 = 1;
        T a1 = 0;
        T a2 = 0;

        void clear() {*this = Self();}
    };

    template <typename T>
    std::ostream &operator<<(std::ostream &os, const Coefficients<T> &coeffs)
    {
        os << "[biquad::Coefficients]";
        os << "(b0:" << coeffs.b0 << ")";
        os << "(b1:" << coeffs.b1 << ")";
        os << "(b2:" << coeffs.b2 << ")";
        os << "(a0:" << coeffs.a0 << ")";
        os << "(a1:" << coeffs.a1 << ")";
        os << "(a2:" << coeffs.a2 << ")";
        return os;
    }

} } 

#endif
