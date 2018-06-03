#include "gubg/graph/detail/Edge.hpp"
#include "catch.hpp"

namespace {

    template <typename VL, typename EL> struct V;
    template <typename VL, typename EL> struct E;

    template <typename VL, typename EL> struct Config
    {
        using VD = typename std::vector<V<VL,EL>>::size_type;
        using ED = typename std::vector<E<VL,EL>>::size_type;
    };

    template <typename VL, typename EL> struct V
    {
        using CFG = Config<VL, EL>;
        std::vector<typename CFG::ED> edges;
    };
    template <typename VL, typename EL> struct E
    {
        using CFG = Config<VL, EL>;
        typename CFG::VD src;
        typename CFG::VD dst;

    };

    template <typename VL, typename EL>
    struct G
    {
        using CFG = Config<VL, EL>;
        std::vector<V<VL,EL>> vertices;
        std::vector<E<VL,EL>> edges;

    };

    template <template <std::size_t> typename N, typename IntegerList> struct Expand;
    template <template <std::size_t> typename N, std::size_t ... Idx> 
    struct Expand<N, std::index_sequence<Idx...>>
    {
        using type = std::tuple<N<Idx>...>;
    };

    template <std::size_t N, typename VertexLabels = gubg::graph::NoLabel, typename EdgeLabels = gubg::graph::NoLabel>
    struct Graph
    {
        template <std::size_t N1> using VertexLabel = typename VertexLabels::template apply<N1>::type;
        template <std::size_t N1, std::size_t N2> using EdgeLabel = typename EdgeLabels::template apply<N1, N2>::type;
        template <std::size_t N1> using VertexContainer = gubg::graph::detail::VectorContainer<VertexLabel<N1>, gubg::graph::detail::PartitionElement<N1>::template Apply>;
        
        typename Expand<VertexContainer, std::make_index_sequence<N>>::type vertices_;
        
        //template  <std::size_t N> 
        //using Vertex = gubg::graph::detail::Vertex<VertexLabel<N>, >
        //std::make_index_sequence<N>;
        G<std::string, bool> g;
    };

}

TEST_CASE("graph", "[ut][graph]")
{
    Graph<2, gubg::graph::NoLabel, gubg::graph::NoLabel> g;
    std::get<0>(g.vertices_).descriptor_range();

}
