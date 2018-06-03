#ifndef HEADER_gubg_graph_detail_Edge_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_detail_Edge_hpp_ALREADY_INCLUDED

#include "gubg/iterator/Transform.hpp"
#include <tuple>
#include <vector>

namespace gubg { namespace graph { namespace detail {

    struct UndirectedT {};
    struct DirectedT {};
    struct BidirectionalT {};
    struct NoLabelT {};

    template <typename T> struct Label { T value; };
    template <> struct Label<NoLabelT> {};

    template <template <typename> typename T, typename ... EdgeDescriptors> struct EdgesContainer
    {
        typedef std::tuple<T<EdgeDescriptors>...> type;
    };

    template <typename VertexLabel, template <typename T> typename EdgeList, typename DirectionMode, typename ... EdgeDescriptors> struct Vertex;

    template <typename VertexLabel, template <typename T> typename EdgeList, typename ... EdgeDescriptors> 
    struct Vertex<VertexLabel, EdgeList, UndirectedT, EdgeDescriptors...> : public Label<VertexLabel>
    {
        EdgesContainer<EdgeList, EdgeDescriptors...> out_;
    };
    template <typename VertexLabel, template <typename T> typename EdgeList, typename ... EdgeDescriptors> 
    struct Vertex<VertexLabel, EdgeList, DirectedT, EdgeDescriptors...>  : public Label<VertexLabel>
    {
        EdgesContainer<EdgeList, EdgeDescriptors...> out_;
    };
    template <typename VertexLabel, template <typename T> typename EdgeList, typename ... EdgeDescriptors> 
    struct Vertex<VertexLabel, EdgeList, BidirectionalT, EdgeDescriptors...> : public Label<VertexLabel>
    {
        EdgesContainer<EdgeList, EdgeDescriptors...> out_;
        EdgesContainer<EdgeList, EdgeDescriptors...> in_;
    };

    template <typename EdgeLabel, typename SourceDescriptor, typename TargetDescriptor>
    struct Edge : public Label<EdgeLabel>
    {
        SourceDescriptor src_;
        TargetDescriptor tgt_;
    };

    template <typename T, template <typename> class DescriptorTransformer> struct VectorContainer
    {
        using container_type = std::vector<T>;
        using descriptor_transformer = DescriptorTransformer<typename container_type::size_type>;
        using descriptor = typename descriptor_transformer::type;
        using element_iterator = typename container_type::const_iterator;
        struct Transformer 
        {
            Transformer(element_iterator begin)
            : begin_(begin)
            {
            }

            descriptor operator()(element_iterator current) const
            {
                return std::distance(begin_, current);
            }
            element_iterator begin_;
        };
        using descriptor_iterator = gubg::iterator::Transform<element_iterator, Transformer, false>;

        gubg::Range<element_iterator> element_range() const { return gubg::make_range(ctr_); }
        gubg::Range<descriptor_iterator> descriptor_range() const 
        { 
            Transformer trans(ctr_.begin());
            return gubg::make_range(descriptor_iterator(ctr_.begin(), trans), descriptor_iterator(ctr_.end(), trans));
        }

        container_type ctr_;
    };

    template <std::size_t ... N> struct PartitionElement
    {
        template <typename T> struct Apply
        {
            struct type
            {
                type(const T & t) : t_(t) {}
                type(T && t) : t_(std::move(t)) {}

                operator T &() { return t_; }
                operator const T &() const { return t_; }

                T t_;
            };
        };
    };

} 

struct NoLabel
{
    template <std::size_t ... N> struct apply
    {
        typedef NoLabel type;
    };
};


} }

#endif

