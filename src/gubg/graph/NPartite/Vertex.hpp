#ifndef HEADER_gubg_graph_NPartite_Vertex_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_NPartite_Vertex_hpp_ALREADY_INCLUDED

#include "gubg/graph/NPartite/Forward.hpp"
#include "gubg/graph/NPartite/Tag.hpp"
#include "gubg/graph/NPartite/Config.hpp"
#include "gubg/graph/NPartite/TupleFold.hpp"

namespace gubg { namespace graph { namespace NPartite {

    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, UndirectedT, VertexListT, EdgeListT, P>
    {
        using Config = NPartite::Config<N, VertexLabels, EdgeLabels, UndirectedT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
    };
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, DirectedT, VertexListT, EdgeListT, P>
    {
        using Config = NPartite::Config<N, VertexLabels, EdgeLabels, DirectedT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
    };
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename VertexListT, typename EdgeListT, std::size_t P> 
    struct Vertex<N, VertexLabels, EdgeLabels, BidirectionalT, VertexListT, EdgeListT, P>
    {
        using Config = NPartite::Config<N, VertexLabels, EdgeLabels, BidirectionalT, VertexListT, EdgeListT>;
        typename Config::template OutEdgeTuple<P> out_;
        typename Config::template InEdgeTuple<P> in_;
    };

} } }

#endif

