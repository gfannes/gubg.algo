#ifndef HEADER_gubg_RMS_hpp_ALREADY_INCLUDED
#define HEADER_gubg_RMS_hpp_ALREADY_INCLUDED

#include <cmath>

namespace gubg { 

    template <typename T>
    class RMS
    {
    public:
        using Self = RMS<T>;

        void clear() {*this = Self{};}

        T linear() const { return std::sqrt(sum_squares_/size_); }
        T db() const {return 20.0*std::log10(linear());}

        void process(const T *begin, const T *end)
        {
            for (auto it = begin; it != end; ++it)
            {
                const auto v = *it;
                sum_squares_ += v*v;
            }
            size_ += end-begin;
        }
        void process(T v)
        {
            sum_squares_ += v*v;
            ++size_;
        }

    private:
        T sum_squares_ = 0;
        unsigned int size_ = 0;
    };

} 

#endif
