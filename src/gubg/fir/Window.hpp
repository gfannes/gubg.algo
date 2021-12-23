#ifndef HEADER_gubg_fir_Window_hpp_ALREADY_INCLUDED
#define HEADER_gubg_fir_Window_hpp_ALREADY_INCLUDED

#include <gubg/math/constants.hpp>
#include <cmath>
#include <numeric>
#include <string>

namespace gubg { namespace fir { 

    enum class Window
    {
        Rectangular,
        Hann,
    };

    inline bool from_string(Window &window, const std::string &str)
    {
        if (false) ;
        else if (str == "Rectangular") window = Window::Rectangular;
        else if (str == "Hann") window = Window::Hann;
        else return false;
        return true;
    }

    template <typename T>
    bool create_window(Window window, T *ptr, std::size_t size, bool normalize)
    {
        MSS_BEGIN(bool);

        MSS(!!ptr);
        MSS(size > 0);

        switch (window)
        {
            case Window::Rectangular:
            std::fill(ptr, ptr+size, 1);
            break;

            case Window::Hann:
            for (auto ix = 0u; ix < size; ++ix)
            {
                const auto s = std::sin(gubg::math::pi*(ix+1)/(size+1));
                const auto ss = s*s;
                ptr[ix] = ss;
            }
            break;

            default: MSS(false); break;
        }

        if (normalize)
        {
            const auto sum = std::accumulate(ptr, ptr+size, T{});
            MSS(sum > 0);
            const auto factor = 1.0/sum;
            for (auto ix = 0u; ix < size; ++ix)
                ptr[ix] *= factor;
        }

        MSS_END();
    }

} } 

#endif