#ifndef HEADER_gubg_graph_NPartite_Forward_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_NPartite_Forward_hpp_ALREADY_INCLUDED

#include <utility>

namespace gubg { namespace graph { namespace NPartite {

    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT> struct Graph;
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT> struct Config;
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t P> struct Vertex;
    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename OutEdgeListT, typename VertexListT, typename EdgeListT, std::size_t P1, std::size_t P2> struct Edge;

} } }

#endif

