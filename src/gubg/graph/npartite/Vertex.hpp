#ifndef HEADER_gubg_graph_npartite_Vertex_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_npartite_Vertex_hpp_ALREADY_INCLUDED

#include "gubg/graph/npartite/Forward.hpp"
#include "gubg/graph/npartite/Tag.hpp"
#include "gubg/graph/npartite/Config.hpp"
#include "gubg/graph/npartite/TupleFold.hpp"

namespace gubg { namespace graph { namespace npartite {

    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, UndirectedT, OutEdgeListT, VertexListT, EdgeListT, P>
    {
        using Config = npartite::Config<N, VertexLabels, EdgeLabels, UndirectedT, OutEdgeListT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
    };
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, DirectedT, OutEdgeListT, VertexListT, EdgeListT, P>
    {
        using Config = npartite::Config<N, VertexLabels, EdgeLabels, DirectedT, OutEdgeListT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
    };
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, BidirectionalT, OutEdgeListT, VertexListT, EdgeListT, P>
    {
        using Config = npartite::Config<N, VertexLabels, EdgeLabels, BidirectionalT, OutEdgeListT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
        typename Config::template InEdgeTuple<P> in_;
    };

} } }

#endif

