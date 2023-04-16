#include <gubg/graph/Graph.hpp>
#include <gubg/graph/detail/Edge.hpp>
#include <gubg/graph/search/Dependency.hpp>
#include <gubg/hr.hpp>
#include <gubg/thread/Pool.hpp>

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
        std::optional<std::size_t> vertex_count;
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
    SECTION("random")
    {
        scn.vertex_count = 1000000;
        exp.vertex_count = 1000000;
    }

    if (scn.vertex_count)
    {
        const std::size_t v_count = *scn.vertex_count;
        const graph::Vertex v_max = v_count - 1;

        std::random_device rd;
        std::mt19937 rng(rd());

        for (auto v = 0; v < v_count; ++v)
        {
            const auto n = 20;
            auto &outs = scn.als[v];
            const auto my_v_max = std::min<graph::Vertex>(v_max, v + n);
            auto v_prev = v;
            while (v_prev < my_v_max && outs.size() < n)
            {
                std::uniform_int_distribution<> uniform(v_prev + 1, my_v_max);

                const auto v = uniform(rng);
                outs.push_back(v);
                v_prev = v;
            }
        }
    }

    graph::Graph g;
    g.init(scn.als, true);
    REQUIRE(g.valid());
    REQUIRE(g.vertex_count() == exp.vertex_count);
    if (g.vertex_count() < 100)
        std::cout << g;

    graph::search::Dependency depsearch;
    REQUIRE(depsearch.init(&g));
    REQUIRE(depsearch.valid());
    depsearch.reset();
    REQUIRE(depsearch.valid());

    graph::Vertices order_st;
    const auto topo_order_ok = depsearch.topo_order(order_st);
    REQUIRE(topo_order_ok == exp.topo_order_ok);
    if (topo_order_ok)
    {
        if (g.vertex_count() < 100)
            std::cout << "ST: " << hr(order_st) << std::endl;

        graph::Vertices order_mt;
        depsearch.reset();
        REQUIRE(depsearch.valid());
        for (graph::Vertex_opt v; depsearch.next_mt(v);)
        {
            if (v)
            {
                order_mt.push_back(*v);
                depsearch.done_mt(*v);
            }
        }
        if (g.vertex_count() < 100)
            std::cout << "MT: " << hr(order_mt) << std::endl;

        REQUIRE(order_st == order_mt);

        graph::Vertices order_mt2;
        {
            std::mutex mutex;
            auto process_vertex = [&](graph::Vertex v) {
                S("process_vertex");
                L(C(v));
                std::unique_lock<std::mutex> lock{mutex};
                order_mt2.push_back(v);
                depsearch.done_mt(v);
            };

            thread::Pool<graph::Vertex> tp{10, process_vertex};

            depsearch.reset();
            REQUIRE(depsearch.valid());
            graph::Vertices vs;
            for (graph::Vertex_opt v; depsearch.next_mt(v);)
            {
                if (v)
                {
                    vs.resize(0);
                    vs.push_back(*v);
                    while (depsearch.next_mt(v) && v)
                        vs.push_back(*v);

                    if (vs.size() >= tp.size())
                        tp.push_many(vs);
                    else
                    {
                        for (auto v : vs)
                            tp.push_one(v);
                    }
                }
            }
        }
        if (g.vertex_count() < 100)
            std::cout << "MT: " << hr(order_mt2) << std::endl;

        REQUIRE(order_mt2.size() == order_st.size());

        std::sort(RANGE(order_st));
        std::sort(RANGE(order_mt));
        std::sort(RANGE(order_mt2));
        REQUIRE(order_mt == order_st);
        REQUIRE(order_mt2 == order_st);
    }
}
