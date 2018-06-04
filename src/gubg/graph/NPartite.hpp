#ifndef HEADER_gubg_graph_NPartite_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_NPartite_hpp_ALREADY_INCLUDED

#include "gubg/graph/npartite/Vertex.hpp"
#include "gubg/graph/npartite/Edge.hpp"
#include "gubg/graph/npartite/Config.hpp"
#include "gubg/graph/container_dispatch/Vector.hpp"

namespace gubg { namespace graph {

    using NoLabel = npartite::NoLabel;
    using VectorT = container_dispatch::VectorT;
    using DirectedT = npartite::DirectedT;
    using UndirectedT = npartite::UndirectedT;
    using BidirectionalT = npartite::BidirectionalT;
    
    template <std::size_t N, typename VertexLabels = NoLabel, typename EdgeLabels = NoLabel, typename DirectionMode = UndirectedT, typename OutEdgeListT = VectorT, typename VertexListT = VectorT, typename EdgeListT = VectorT> 
    struct NPartite
    {
        using Config = npartite::Config<N, VertexLabels, EdgeLabels, DirectionMode, OutEdgeListT, VertexListT, EdgeListT>;

        typename Config::VertexTuple vertices_;
        typename Config::EdgeTuple edges_;
    };


} }

#endif

