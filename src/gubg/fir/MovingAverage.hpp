#ifndef HEADER_gubg_fir_MovingAverage_hpp_ALREADY_INCLUDED
#define HEADER_gubg_fir_MovingAverage_hpp_ALREADY_INCLUDED

#include <gubg/fir/Window.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace gubg { namespace fir { 

    template <typename T>
    class MovingAverage
    {
    public:
        bool setup(unsigned int order, Window window)
        {
            MSS_BEGIN(bool);

            MSS(order > 0);
            order_ = order;

            //Twice the order_ to make sure we can always read order_ samples at &history_[ix_]
            history_.resize(2*order_);
            std::fill(history_.begin(), history_.end(), 0);

            window_.resize(order_);
            MSS(create_window(window, window_.data(), window_.size(), true));

            ix_ = 0;

            MSS_END();
        }

        //Assumes setup() was called successfully
        T operator()(T v)
        {
            assert(order_ > 0);
            assert(ix_ < order_);

            history_[ix_+order_] = history_[ix_] = v;

            T res = T{};
            for (auto offset = 0u; offset < order_; ++offset)
            {
                const auto w = window_[offset];
                const auto h = history_[ix_+offset];
                res += window_[offset]*history_[ix_+offset];
            }

            ix_ = (++ix_ == order_) ? 0u : ix_;

            return res;
        }

    private:
        unsigned int order_ = 0u;
        std::vector<T> history_;
        std::vector<T> window_;
        std::size_t ix_ = 0;
    };

} } 

#endif