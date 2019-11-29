#ifndef HEADER_gubg_fir_Filter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_fir_Filter_hpp_ALREADY_INCLUDED

#include <gubg/History.hpp>
#include <vector>

namespace gubg { namespace fir { 

    template <typename T>
    class Filter
    {
    public:
        template <typename Coeffs>
        Filter(const Coeffs &coeffs): coeffs_(coeffs.size())
        {
            std::copy(coeffs.begin(), coeffs.end(), coeffs_.begin());
            history_.resize(coeffs.size());
        }

        T operator()(T v)
        {
            history_.push_pop(v);
            const T *hist = &history_[0];
            const T *coef = &coeffs_[0];
            v = T{};
            const auto size = coeffs_.size();
            for (auto ix = 0u; ix < size; ++ix)
                v += coef[ix]*hist[ix];
            return v;
        }

    private:
        std::vector<T> coeffs_;
        History<T> history_;
    };

} } 

#endif
