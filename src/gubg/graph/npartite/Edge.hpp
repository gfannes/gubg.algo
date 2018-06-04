#ifndef HEADER_gubg_graph_npartite_Edge_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_npartite_Edge_hpp_ALREADY_INCLUDED

#include "gubg/graph/npartite/Forward.hpp"
#include "gubg/graph/npartite/Tag.hpp"
#include "gubg/graph/npartite/Config.hpp"
#include "gubg/graph/npartite/Label.hpp"

namespace gubg { namespace graph { namespace npartite {

    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename DirectionMode, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t SRC, std::size_t TGT> 
    struct Edge : public Label<EdgeLabels, SRC, TGT>
    {
        using Config = npartite::Config<N, VertexLabels, EdgeLabels, DirectionMode, OutEdgeListT, VertexListT, EdgeListT>;
        typename Config::template vertex_descriptor<SRC> src;
        typename Config::template vertex_descriptor<TGT> tgt;
    };

} } }

#endif

