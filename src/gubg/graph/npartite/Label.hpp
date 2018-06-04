#ifndef HEADER_gubg_graph_npartite_Label_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_npartite_Label_hpp_ALREADY_INCLUDED

#include <utility>

namespace gubg { namespace graph { namespace npartite {

    struct NoLabel
    {
        template <std::size_t ...> struct apply
        {
            using type = NoLabel;
        };
    };

    template <typename T> struct Labeled
    {
        Labeled(const T & label = T()) : label_(label) {}
        Labeled(T && label) : label_(std::move(label)) {}

        T label_;
    };
    
    template <> struct Labeled<NoLabel>
    {
    };

    template <typename LabelGenerator, std::size_t ... Ps> 
    using Label = Labeled<typename LabelGenerator::template apply<Ps...>::type>;


} } }

#endif

