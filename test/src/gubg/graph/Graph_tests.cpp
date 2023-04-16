#include <gubg/graph/Graph.hpp>
#include <gubg/graph/detail/Edge.hpp>
#include <gubg/hr.hpp>

#include <catch.hpp>

#include <iostream>

using namespace gubg;

namespace {

    template<typename VL, typename EL>
    struct V;
    template<typename VL, typename EL>
    struct E;

    template<typename VL, typename EL>
    struct Config
    {
        using VD = typename std::vector<V<VL, EL>>::size_type;
        using ED = typename std::vector<E<VL, EL>>::size_type;
    };

    template<typename VL, typename EL>
    struct V
    {
        using CFG = Config<VL, EL>;
        std::vector<typename CFG::ED> edges;
    };
    template<typename VL, typename EL>
    struct E
    {
        using CFG = Config<VL, EL>;
        typename CFG::VD src;
        typename CFG::VD dst;
    };

    template<typename VL, typename EL>
    struct G
    {
        using CFG = Config<VL, EL>;
        std::vector<V<VL, EL>> vertices;
        std::vector<E<VL, EL>> edges;
    };

    template<template<std::size_t> typename N, typename IntegerList>
    struct Expand;
    template<template<std::size_t> typename N, std::size_t... Idx>
    struct Expand<N, std::index_sequence<Idx...>>
    {
        using type = std::tuple<N<Idx>...>;
    };

    template<std::size_t N, typename VertexLabels = gubg::graph::NoLabel, typename EdgeLabels = gubg::graph::NoLabel>
    struct Graph
    {
        template<std::size_t N1>
        using VertexLabel = typename VertexLabels::template apply<N1>::type;
        template<std::size_t N1, std::size_t N2>
        using EdgeLabel = typename EdgeLabels::template apply<N1, N2>::type;
        template<std::size_t N1>
        using VertexContainer = gubg::graph::detail::VectorContainer<VertexLabel<N1>, gubg::graph::detail::PartitionElement<N1>::template Apply>;

        typename Expand<VertexContainer, std::make_index_sequence<N>>::type vertices_;

        // template  <std::size_t N>
        // using Vertex = gubg::graph::detail::Vertex<VertexLabel<N>, >
        // std::make_index_sequence<N>;
        G<std::string, bool> g;
    };

} // namespace

TEST_CASE("graph", "[ut][graph]")
{
    Graph<2, gubg::graph::NoLabel, gubg::graph::NoLabel> g;
    std::get<0>(g.vertices_).descriptor_range();
}

TEST_CASE("graph::Graph tests", "[ut][graph][Graph]")
{
    struct Scn
    {
        graph::AdjacencyLists als;
    };
    Scn scn;

    struct Exp
    {
        std::size_t vertex_count = 0;
        bool topo_order_ok = true;
    };
    Exp exp;

    SECTION("empty")
    {}
    SECTION("a")
    {
        exp.vertex_count = 3;

        scn.als[0] = {1, 2};
        scn.als[1] = {2};
    }
    SECTION("b")
    {
        exp.vertex_count = 3;

        scn.als[0] = {1, 2};
        scn.als[2] = {1};
    }
    SECTION("c")
    {
        exp.vertex_count = 2;
        exp.topo_order_ok = false;

        scn.als[0] = {1};
        scn.als[1] = {0};
    }

    graph::Graph g;
    g.init(scn.als);
    REQUIRE(g.valid());
    REQUIRE(g.vertex_count() == exp.vertex_count);
    std::cout << g;

    graph::Vertices order;
    const auto topo_order_ok = g.topo_order(order);
    REQUIRE(topo_order_ok == exp.topo_order_ok);
    if (topo_order_ok)
        std::cout << hr(order) << std::endl;
}
