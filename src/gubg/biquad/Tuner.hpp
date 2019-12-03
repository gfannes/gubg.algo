#ifndef HEADER_gubg_biquad_Tuner_hpp_ALREADY_INCLUDED
#define HEADER_gubg_biquad_Tuner_hpp_ALREADY_INCLUDED

//Info from http://shepazu.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

#include <gubg/biquad/Coefficients.hpp>
#include <gubg/mss.hpp>
#include <gubg/math/constants.hpp>
#include <cmath>

namespace gubg { namespace biquad { 

    //a0*y[n-0] + a1*y[n-1] + a2*y[n-2] = b0*x[n-0] + b1*x[n-1] + b2*x[n-2]
    //=> y[n-0] = (b0*x[n-0] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2])/a0
    //=> H(z) = (b0 + b1*z^-1 + b2*z^-2)/(a0 + a1*z^-1 + a2*z^-2)

    enum class Type
    {
        Bypass,
        HighPass,
        LowPass,
        BandPassSkirt,
        BandPass0dB,
        Notch,
        AllPass,
        Peak,
        LowShelf,
        HighShelf,

        Nr_,
    };

    template <typename T>
    class Tuner
    {
    public:
        using Coefficients = biquad::Coefficients<T>;

        Tuner(T samplerate): samplerate_(samplerate)
        {
            configure(0, 1, Type::Bypass);
            set_gain_db(0);
        }

        void configure(T frequency, T q, Type type)
        {
            frequency_ = frequency;
            q_ = q;
            type_ = type;
        }

        void set_gain_db(T gain_db)
        {
            gain_db_ = gain_db;
            gain_linear_ = std::pow(10.0, gain_db_/20.0);
            a_ = std::pow(10.0, gain_db_/40.0);
        }

        //Returned memory remains valid until the next invocation of compute()
        Coefficients *compute()
        {
            if (!compute_())
                return nullptr;
            return &coeffs_;
        }

    private:
        bool compute_()
        {
            MSS_BEGIN(bool);

            MSS(samplerate_ > 0);
            const T w0 = math::tau*frequency_/samplerate_;

            const T sin_w0 = std::sin(w0);
            const T cos_w0 = std::cos(w0);

            MSS(q_ > 0);
            const T alpha = sin_w0/2/q_;

            coeffs_.clear();
            coeffs_.a0 = 1;
            T &b0 = coeffs_.b0;
            T &b1 = coeffs_.b1;
            T &b2 = coeffs_.b2;
            T &a0 = coeffs_.a0;
            T &a1 = coeffs_.a1;
            T &a2 = coeffs_.a2;
            switch (type_)
            {
                case Type::Bypass:
                    b0 = gain_linear_;
                    break;
                case Type::LowPass:
                    //H(z) = 1/(z^2+z/q+1)
                    {
                        b0 = 0.5*(1-cos_w0);
                        b1 = 1-cos_w0;
                        b2 = 0.5*(1-cos_w0);
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::HighPass:
                    //H(z) = z^2/(z^2+z/q+1)
                    {
                        b0 = 0.5*(1+cos_w0);
                        b1 = cos_w0-1;
                        b2 = 0.5*(1+cos_w0);
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::BandPassSkirt:
                    //H(z) = z/(z^2+z/q+1)
                    {
                        b0 = q_*alpha;
                        b1 = 0;
                        b2 = -q_*alpha;
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::BandPass0dB:
                    //H(z) = (z/q)/(z^2+z/q+1)
                    {
                        b0 = alpha;
                        b1 = 0;
                        b2 = -alpha;
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::Notch:
                    //H(z) = (z^2+1)/(z^2+z/q+1)
                    {
                        b0 = 1;
                        b1 = -2*cos_w0;
                        b2 = 1;
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::AllPass:
                    //H(z) = (z^2-z/q+1)/(z^2+z/q+1)
                    {
                        b0 = 1-alpha;
                        b1 = -2*cos_w0;
                        b2 = 1+alpha;
                        a0 = 1+alpha;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha;
                    }
                    break;
                case Type::Peak:
                    //H(z) = (z^2+z*a/q+1)/(z^2+z/a/q+1)
                    {
                        MSS(a_ > 0);
                        b0 = 1+alpha*a_;
                        b1 = -2*cos_w0;
                        b2 = 1-alpha*a_;
                        a0 = 1+alpha/a_;
                        a1 = -2*cos_w0;
                        a2 = 1-alpha/a_;
                    }
                    break;
                case Type::LowShelf:
                    //H(z) = a*(z^2+z*sqrt(a)/q+a)/(a*z^2+z*sqrt(a)/q+1)
                    {
                        const T sqrt_a = std::sqrt(a_);
                        b0 = a_*((a_+1)-(a_-1)*cos_w0+2*sqrt_a*alpha);
                        b1 = 2*a_*((a_-1)-(a_+1)*cos_w0);
                        b2 = a_*((a_+1)-(a_-1)*cos_w0-2*sqrt_a*alpha);
                        a0 = ((a_+1)+(a_-1)*cos_w0+2*sqrt_a*alpha);
                        a1 = -2*((a_-1)+(a_+1)*cos_w0);
                        a2 = ((a_+1)+(a_-1)*cos_w0-2*sqrt_a*alpha);
                    }
                    break;
                case Type::HighShelf:
                    //H(z) = a*(a*z^2+z*sqrt(a)/q+a)/(z^2+z*sqrt(a)/q+a)
                    {
                        const T sqrt_a = std::sqrt(a_);
                        b0 = a_*((a_+1)+(a_-1)*cos_w0+2*sqrt_a*alpha);
                        b1 = -2*a_*((a_-1)+(a_+1)*cos_w0);
                        b2 = a_*((a_+1)+(a_-1)*cos_w0-2*sqrt_a*alpha);
                        a0 = ((a_+1)-(a_-1)*cos_w0+2*sqrt_a*alpha);
                        a1 = 2*((a_-1)-(a_+1)*cos_w0);
                        a2 = ((a_+1)-(a_-1)*cos_w0-2*sqrt_a*alpha);
                    }
                    break;
            }

            MSS_END();
        }

        Coefficients coeffs_;

        T samplerate_;

        T frequency_ = 0;
        T q_ = 1;
        Type type_ = Type::Bypass;

        T gain_db_;
        T gain_linear_;
        T a_;
    };

} } 

#endif
