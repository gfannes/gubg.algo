#ifndef HEADER_gubg_signal_LinearChirp_hpp_ALREADY_INCLUDED
#define HEADER_gubg_signal_LinearChirp_hpp_ALREADY_INCLUDED

#include <gubg/math/constants.hpp>
#include <cmath>

namespace gubg { namespace signal { 

    //phase(0) = start_phase
    //frequency(t) = start_frequency + c*t
    //phase(t) = start_phase + 2*pi*(start_frequency*t + c/2*t*t)
    //         = x_ + y_*t + z_*t*t
    //signal(t) = amplitude*sine(phase(t))

    template <typename T>
    class LinearChirp
    {
    public:
        LinearChirp(T amplitude, T start_phase, T start_frequency, T end_frequency, T duration): amplitude_(amplitude), x_(start_phase), y_(math::tau*start_frequency), z_(math::pi*(end_frequency-start_frequency)/duration)
        {
        }

        T operator()(T t) const
        {
            return amplitude_*std::sin(x_+(y_+z_*t)*t);
        }

    private:
        const T amplitude_;
        const T x_, y_, z_;
    };

} } 

#endif
