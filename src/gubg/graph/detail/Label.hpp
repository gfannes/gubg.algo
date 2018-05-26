#ifndef HEADER_gubg_graph_detail_Label_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_detail_Label_hpp_ALREADY_INCLUDED

#include "gubg/graph/Tag.hpp"
#include <utility>

namespace gubg { namespace graph { namespace detail {

    template <typename LabelType> struct Label
    {
        explicit Label(const LabelType & label = LabelType())
        : label_(label)
        {
        }

        explicit Label(LabelType && label)
        : label_(std::move(label))
        {
        }

        LabelType label_;
    };

    template <> struct Label<no_label>
    {
        explicit Label(no_label)
        {
        }
    };

} } }

#endif

