#ifndef HEADER_gubg_fir_invert_hpp_ALREADY_INCLUDED
#define HEADER_gubg_fir_invert_hpp_ALREADY_INCLUDED

namespace gubg { namespace fir { 

    template <typename Dst, typename Src>
    void reverse_frequency(Dst &dst, const Src &src)
    {
        const auto size = src.size();
        dst.resize(size);
        for (auto ix = 0u; ix < size; ++ix)
            dst[ix] = (ix%2 == 0 ? src[ix] : -src[ix]);
    }

} } 

#endif
