#ifndef HEADER_gubg_graph_AdjacencyList_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_AdjacencyList_hpp_ALREADY_INCLUDED

#include "gubg/graph/Tag.hpp"
#include "gubg/graph/detail/Label.hpp"
#include "gubg/graph/detail/ContainerDispatch.hpp"
#include "gubg/Range.hpp"

namespace gubg { namespace graph { namespace detail {

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename Direction>
    struct Edge;

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename Direction>
    struct Vertex;

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename Direction>
    struct Config
    {
        using vertex_type = Vertex<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, Direction>;
        using edge_type = Edge<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, Direction>;

        // vertex list info
        using vertex_list_type          = typename ContainerDispatch<VertexListTag, vertex_type>::type;
        using vertex_descriptor         = typename vertex_list_type::descriptor;

        // edge list info
        using edge_list_type            = typename ContainerDispatch<EdgeListTag, edge_type>::type;
        using edge_descriptor           = typename edge_list_type::descriptor;

        // out edge list info
        using out_edge_list_type        = typename ContainerDispatch<OutEdgeListTag, edge_descriptor>::type;
        using out_edge_descriptor       = typename out_edge_list_type::descriptor;
    };


    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename Direction>
    struct Edge : public Label<EdgeLabel>
    {
        using config = Config<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, Direction>;
        using vertex_descriptor = typename config::vertex_descriptor;

        explicit Edge(vertex_descriptor u, vertex_descriptor v, const EdgeLabel & label = EdgeLabel()) 
        : Label<EdgeLabel>(label),
        src(u),
        tgt(v)
        {
        }

        explicit Edge(vertex_descriptor u, vertex_descriptor v, EdgeLabel && label) 
        : Label<EdgeLabel>(std::move(label)),
        src(u),
        tgt(v)
        {
        }

        vertex_descriptor source() const { return src; }
        vertex_descriptor target() const { return tgt; }

        vertex_descriptor src;
        vertex_descriptor tgt;
    };


    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel>
    struct Vertex<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, undirected> : public Label<VertexLabel>
    {
        using config = Config<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, undirected>;
        using container_type = typename config::out_edge_list_type;

        explicit Vertex(const VertexLabel & label = VertexLabel())
        : Label<VertexLabel>(label)
        {
        }

        explicit Vertex(VertexLabel && label)
        : Label<VertexLabel>(std::move(label))
        {
        }

        void add_outgoing(typename config::edge_descriptor edge)
        {
            out.append(edge);
        }

        void add_incoming(typename config::edge_descriptor edge)
        {
            out.append(edge);
        }
        
        Range<typename container_type::element_iterator> out_edges() const
        {
            return out.element_range();
        }
        
        Range<typename container_type::element_iterator> in_edges() const
        {
            return out_edges();
        }

        typename container_type::size_type out_degree() const { return out.size(); }
        typename container_type::size_type in_degree() const { return out_degree(); }

        container_type out;
    };

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel>
    struct Vertex<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, directed> : public Label<VertexLabel>
    {
        using config = Config<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, directed>;
        using container_type = typename config::out_edge_list_type;

        explicit Vertex(const VertexLabel & label = VertexLabel())
        : Label<VertexLabel>(label)
        {
        }

        explicit Vertex(VertexLabel && label)
        : Label<VertexLabel>(std::move(label))
        {
        }

        void add_outgoing(typename config::edge_descriptor edge)
        {
            out.append(edge);
        }

        void add_incoming(typename config::edge_descriptor edge)
        {
        }

        Range<typename container_type::element_iterator> out_edges() const
        {
            return out.element_range();
        }

        typename container_type::size_type out_degree() const { return out.size(); }

        container_type out;
    };

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel>
    struct Vertex<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, bidirectional> : public Label<VertexLabel>
    {
        using config = Config<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, bidirectional>;
        using container_type = typename config::out_edge_list_type;

        explicit Vertex(const VertexLabel & label = VertexLabel())
        : Label<VertexLabel>(label)
        {
        }

        explicit Vertex(VertexLabel && label)
        : Label<VertexLabel>(std::move(label))
        {
        }

        void add_outgoing(typename config::edge_descriptor edge)
        {
            out.append(edge);
        }

        void add_incoming(typename config::edge_descriptor edge)
        {
            in.append(edge);
        }
        
        Range<typename container_type::element_iterator> out_edges() const
        {
            return out.element_range();
        }

        Range<typename container_type::element_iterator> in_edges() const
        {
            return in.element_range();
        }
        typename container_type::size_type out_degree() const { return out.size(); }
        typename container_type::size_type in_degree() const { return in.size(); }
        container_type out;
        container_type in;
    };

    template <typename Tag> struct EdgeInformation;
    template <> struct EdgeInformation<undirected>
    {
        static constexpr bool has_direction = false;
        static constexpr bool has_incoming = true;
    };
    template <> struct EdgeInformation<directed>
    {
        static constexpr bool has_direction = true;
        static constexpr bool has_incoming = false;
    };
    template <> struct EdgeInformation<bidirectional>
    {
        static constexpr bool has_direction = true;
        static constexpr bool has_incoming = true;
    };

    template <typename Graph> struct SourceVertexTransformer
    {
        SourceVertexTransformer(const Graph * g)
        : g_(g)
        {
        }

        typename Graph::vertex_descriptor operator()(typename Graph::edge_descriptor e) const
        {
            return g_->source(e);
        }

        const Graph * g_;
    };
    template <typename Graph> struct TargetVertexTransformer
    {
        TargetVertexTransformer(const Graph * g)
        : g_(g)
        {
        }

        typename Graph::vertex_descriptor operator()(typename Graph::edge_descriptor e) const
        {
            return g_->target(e);
        }


        const Graph * g_;
    };
}

template <typename Graph>
constexpr bool has_incoming_edge_information(const Graph & g)
{
    return detail::EdgeInformation<typename Graph::direction_tag>::has_incoming;
}
template <typename Graph>
constexpr bool has_incoming_edge_information()
{
    return detail::EdgeInformation<typename Graph::direction_tag>::has_incoming;
}
template <typename Graph>
constexpr bool has_direction(const Graph & g)
{
    return detail::EdgeInformation<typename Graph::direction_tag>::has_direction;
}

template <typename Graph>
constexpr bool has_direction()
{
    return detail::EdgeInformation<typename Graph::direction_tag>::has_direction;
}


template <
    typename VertexListTag = use_list, 
    typename OutEdgeListTag = use_list, 
    typename EdgeListTag = use_list, 
    typename VertexLabel = no_label, 
    typename EdgeLabel = no_label, 
    typename DirectionTag = undirected
    > class AdjacencyList
{

    using config = detail::Config<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, DirectionTag>;
    using Vertex = typename config::vertex_type;
    using Edge = typename config::edge_type;

    using Graph = AdjacencyList<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, DirectionTag>;

public:
    using vertex_list = typename config::vertex_list_type;
    using edge_list = typename config::edge_list_type;
    using out_edge_list = typename config::out_edge_list_type;
    using vertex_descriptor = typename config::vertex_descriptor;
    using edge_descriptor = typename config::edge_descriptor;
    using degree_type = typename config::out_edge_list_type::size_type;
    using vertices_size_type = typename config::vertex_list_type::size_type;
    using edges_size_type = typename config::edge_list_type::size_type;
    using vertex_iterator = typename vertex_list::descriptor_iterator;
    using edge_iterator = typename edge_list::descriptor_iterator;
    using out_edge_iterator = typename out_edge_list::element_iterator;
    using adjacent_iterator = gubg::iterator::Transform<out_edge_iterator, detail::TargetVertexTransformer<Graph>>;
    using in_edge_iterator = out_edge_list;
    using inv_adjacent_iterator = gubg::iterator::Transform<in_edge_iterator, detail::SourceVertexTransformer<Graph>>;
    using direction_tag = DirectionTag;

    public:
    AdjacencyList() {}
    
    vertices_size_type num_vertices() const
    {
        return vertices_.size();
    }

    edges_size_type num_edges() const
    {
        return edges_.size();
    }

    degree_type out_degree(vertex_descriptor v) const
    {
        return vertices_.get(v).out_degree();
    }

    degree_type in_degree(vertex_descriptor v, std::enable_if<has_incoming_edge_information<Graph>()> * /*dummy*/ = nullptr) const
    {
        return vertices_.get(v).in_degree();
    }

    Range<vertex_iterator> vertices() const
    {
        return vertices_.descriptor_range();
    }
    
    Range<edge_iterator> edges() const 
    {
        return edges_.descriptor_range();
    }
    Range<out_edge_iterator> out_edges(vertex_descriptor v) const
    {
        return vertices_.get(v).out_edges();
    }
    Range<out_edge_iterator> in_edges(vertex_descriptor v, std::enable_if_t<has_incoming_edge_information<Graph>()> * /*dummy*/ = nullptr) const
    {
        return vertices_.get(v).in_edges();
    }
    Range<adjacent_iterator> adjacent_vertices(vertex_descriptor v) const
    {
        using T = detail::TargetVertexTransformer<Graph>;
        return gubg::iterator::transform(out_edges(v), T(this));
    }

    Range<inv_adjacent_iterator> inv_adjacent_vertices(vertex_descriptor v, std::enable_if<has_incoming_edge_information<Graph>()> * /*dummy*/ = nullptr) const
    {
        using T = detail::SourceVertexTransformer<Graph>;
        return gubg::iterator::transform(in_edges(v), T(this));
    }

    vertex_descriptor add_vertex(const VertexLabel & l = VertexLabel())
    {
        return vertices_.append(Vertex(l));
    }
    
    vertex_descriptor add_vertex(VertexLabel && l)
    {
        return vertices_.append(Vertex(std::move(l)));
    }

    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, const EdgeLabel & l = EdgeLabel())
    {
        edge_descriptor e = edges_.append(Edge(u, v, l));

        vertices_.get(u).add_outgoing(e);
        vertices_.get(v).add_incoming(e);

        return e;
    }

    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, EdgeLabel && l)
    {
        edge_descriptor e = edges_.append(Edge(u, v, l));

        vertices_.get(u).add_outgoing(e);
        vertices_.get(v).add_incoming(e);

        return e;
    }

    vertex_descriptor source(edge_descriptor e) const
    {
        return edges_.get(e).source();
    }

    vertex_descriptor target(edge_descriptor e) const
    {
        return edges_.get(e).target();
    }

    const VertexLabel & vertex_label(vertex_descriptor d, std::enable_if_t<!std::is_same<VertexLabel, no_label>::value> * /*dummy*/ = nullptr) const
    {
        return vertices_.get(d).label;
    }

    VertexLabel & vertex_label(vertex_descriptor d, std::enable_if_t<!std::is_same<VertexLabel, no_label>::value> * /*dummy*/ = nullptr) 
    {
        return vertices_.get(d).label;
    }

    const EdgeLabel & edge_label(edge_descriptor d, std::enable_if_t<!std::is_same<EdgeLabel, no_label>::value> * /*dummy*/ = nullptr) const
    {
        return edges_.get(d).label;
    }

    EdgeLabel & edge_label(edge_descriptor d, std::enable_if_t<!std::is_same<EdgeLabel, no_label>::value> * /*dummy*/ = nullptr)
    {
        return edges_.get(d).label;
    }

    private:
    vertex_list vertices_;
    edge_list edges_;
};

} }

#endif

