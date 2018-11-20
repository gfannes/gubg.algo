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
    using vertex_label_type = VertexLabel;
    using edge_label_type = EdgeLabel;

    using info = detail::EdgeInformation<direction_tag>;

    public:
    AdjacencyList() {}
    AdjacencyList(const AdjacencyList &) = delete;
    AdjacencyList(AdjacencyList &&) = default;
    AdjacencyList & operator=(const AdjacencyList &) = delete;
    AdjacencyList & operator=(AdjacencyList &&) = default;

    void clear()
    {
        vertices_.clear();
        edges_.clear();
    }

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

    template <bool has_incoming = info::has_incoming>
    degree_type in_degree(vertex_descriptor v, std::enable_if<has_incoming> * /*dummy*/ = nullptr) const
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
    template <bool has_incoming = info::has_incoming>
    Range<out_edge_iterator> in_edges(vertex_descriptor v, std::enable_if_t<has_incoming> * /*dummy*/ = nullptr) const
    {
        return vertices_.get(v).in_edges();
    }
    Range<adjacent_iterator> adjacent_vertices(vertex_descriptor v) const
    {
        using T = detail::TargetVertexTransformer<Graph>;
        return gubg::iterator::transform(out_edges(v), T(this));
    }

    template <bool has_incoming = info::has_incoming>
    Range<inv_adjacent_iterator> inv_adjacent_vertices(vertex_descriptor v, std::enable_if<has_incoming> * /*dummy*/ = nullptr) const
    {
        using T = detail::SourceVertexTransformer<Graph>;
        return gubg::iterator::transform(in_edges(v), T(this));
    }

    vertex_descriptor add_vertex()
    {
        return vertices_.append(Vertex(VertexLabel()));
    }

    template <typename Label>
    vertex_descriptor add_vertex(Label && l)
    {
        return vertices_.append(Vertex(std::forward<Label>(l)));
    }

    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v)
    {
        edge_descriptor e = edges_.append(Edge(u, v, EdgeLabel()));

        vertices_.get(u).add_outgoing(e);
        vertices_.get(v).add_incoming(e);

        return e;
    }

    template <typename Label>
    edge_descriptor add_edge(vertex_descriptor u, vertex_descriptor v, Label && l)
    {
        edge_descriptor e = edges_.append(Edge(u, v, std::forward<Label>(l)));

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

    template <typename T = VertexLabel>
    const VertexLabel & vertex_label(vertex_descriptor d, std::enable_if_t<!std::is_same<T, no_label>::value> * /*dummy*/ = nullptr) const
    {
        return vertices_.get(d).label_;
    }

    template <typename T = VertexLabel>
    VertexLabel & vertex_label(vertex_descriptor d, std::enable_if_t<!std::is_same<T, no_label>::value> * /*dummy*/ = nullptr) 
    {
        return vertices_.get(d).label_;
    }

    template <typename T = EdgeLabel>
    const EdgeLabel & edge_label(edge_descriptor d, std::enable_if_t<!std::is_same<T, no_label>::value> * /*dummy*/ = nullptr) const
    {
        return edges_.get(d).label_;
    }

    template <typename T = EdgeLabel>
    EdgeLabel & edge_label(edge_descriptor d, std::enable_if_t<!std::is_same<T, no_label>::value> * /*dummy*/ = nullptr)
    {
        return edges_.get(d).label_;
    }

    private:
    vertex_list vertices_;
    edge_list edges_;
};

} }

#include "gubg/graph/Traits.hpp"

namespace gubg { namespace graph {

    #define TDEF template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename DirectionTag>
    #define CDEF AdjacencyList<VertexListTag, OutEdgeListTag, EdgeListTag, VertexLabel, EdgeLabel, DirectionTag>

    TDEF typename Traits<CDEF>::vertices_size_type num_vertices(const CDEF & g)
    {
        return g.num_vertices();
    }

    TDEF typename Traits<CDEF>::edges_size_type num_edges(const CDEF & g)
    {
        return g.num_edges();
    }

    TDEF typename Traits<CDEF>::degree_type out_degree(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g)
    {
        return g.out_degree(v);
    }

    TDEF typename Traits<CDEF>::degree_type in_degree(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g, std::enable_if<Traits<CDEF>::has_incoming_edge_info> * /*dummy*/ = nullptr)
    {
        return g.in_degree(v);
    }

    TDEF Range<typename Traits<CDEF>::vertex_iterator> vertices(const CDEF & g) 
    {
        return g.vertices();
    }

    TDEF Range<typename Traits<CDEF>::edge_iterator> edges(const CDEF & g)
    {
        return g.edges();
    }

    TDEF Range<typename Traits<CDEF>::out_edge_iterator> out_edges(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g)
    {
        return g.out_edges(v);
    }

    TDEF Range<typename Traits<CDEF>::out_edge_iterator> in_edges(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g, std::enable_if_t<Traits<CDEF>::has_incoming_edge_info> * /*dummy*/ = nullptr)
    {
        return g.in_edges(v);
    }

    TDEF Range<typename Traits<CDEF>::adjacent_iterator> adjacent_vertices(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g)
    {
        return g.adjacent_vertices(v);
    }

    TDEF Range<typename Traits<CDEF>::inv_adjacent_iterator> inv_adjacent_vertices(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g, std::enable_if<Traits<CDEF>::has_incoming_edge_info> * /*dummy*/ = nullptr)
    {
        return g.inv_adjacent_vertices(v);
    }
    
    TDEF typename Traits<CDEF>::vertex_descriptor add_vertex(CDEF & g)
    {
        return g.add_vertex();
    }

    
    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename DirectionTag, typename Label>
    typename Traits<CDEF>::vertex_descriptor add_vertex(Label && l, CDEF & g)
    {
        return g.add_vertex(std::forward<Label>(l));
    }

    TDEF typename Traits<CDEF>::edge_descriptor add_edge(typename Traits<CDEF>::vertex_descriptor u, typename Traits<CDEF>::vertex_descriptor v, CDEF & g)
    {
        return g.add_edge(u, v);
    }

    template <typename VertexListTag, typename OutEdgeListTag, typename EdgeListTag, typename VertexLabel, typename EdgeLabel, typename DirectionTag, typename Label>
    typename Traits<CDEF>::edge_descriptor add_edge(typename Traits<CDEF>::vertex_descriptor u, typename Traits<CDEF>::vertex_descriptor v, Label && l, CDEF & g)
    {
        return g.add_edge(u, v, std::forward<Label>(l));
    }


    TDEF typename Traits<CDEF>::vertex_descriptor source(typename Traits<CDEF>::edge_descriptor e, const CDEF & g)
    {
        return g.source(e);
    }

    TDEF typename Traits<CDEF>::vertex_descriptor target(typename Traits<CDEF>::edge_descriptor e, const CDEF & g)
    {
        return g.target(e);
    }

    TDEF const typename Traits<CDEF>::vertex_label_type & vertex_label(typename Traits<CDEF>::vertex_descriptor v, const CDEF & g, std::enable_if_t<Traits<CDEF>::has_vertex_label> * /*dummy*/ = nullptr)
    {
        return g.vertex_label(v);
    }

    TDEF typename Traits<CDEF>::vertex_label_type & vertex_label(typename Traits<CDEF>::vertex_descriptor v, CDEF & g, std::enable_if_t<Traits<CDEF>::has_vertex_label> * /*dummy*/ = nullptr) 
    {
        return g.vertex_label(v);
    }

    TDEF const typename Traits<CDEF>::edge_label_type & edge_label(typename Traits<CDEF>::edge_descriptor e, const CDEF & g, std::enable_if_t<Traits<CDEF>::has_edge_label> * /*dummy*/ = nullptr)
    {
        return g.edge_label(e);
    }

    TDEF typename Traits<CDEF>::edge_label_type & edge_label(typename Traits<CDEF>::edge_descriptor e, CDEF & g, std::enable_if_t<Traits<CDEF>::has_edge_label> * /*dummy*/ = nullptr)
    {
        return g.edge_label(e);
    }

} }

#endif

