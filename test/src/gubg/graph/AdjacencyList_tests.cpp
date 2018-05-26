#include "catch.hpp"
#include "gubg/graph/AdjacencyList.hpp"
#include "gubg/mss.hpp"

namespace  {

    template <typename It1, typename It2>
    bool is_permutation(const gubg::Range<It1> & r1, const gubg::Range<It2> & r2)
    {
        MSS_BEGIN(bool);
        MSS(std::distance(RANGE(r1)) == std::distance(RANGE(r2)));

        for(auto v : r1)
            MSS(std::count(RANGE(r2), v) == 1);

        MSS_END();
    }

    template <typename Graph, typename VD, typename It> void test_in_edges(const Graph & g, VD v, const gubg::Range<It> & in_edges, gubg::graph::directed)
    {
    }

    template <typename Graph, typename VD, typename It> void test_in_edges(const Graph & g, VD v, const gubg::Range<It> & in_edges, gubg::graph::undirected)
    {
        REQUIRE(is_permutation(g.in_edges(v), in_edges));
        typename Graph::degree_type d = std::distance(RANGE(in_edges));
        REQUIRE(g.in_degree(v) == d);

    }

    template <typename Graph, typename VD, typename It> void test_in_edges(const Graph & g, VD v, const gubg::Range<It> & in_edges, gubg::graph::bidirectional)
    {
        REQUIRE(is_permutation(g.in_edges(v), in_edges));
        typename Graph::degree_type d = std::distance(RANGE(in_edges));
        REQUIRE(g.in_degree(v) == d);
    }

    template <typename Graph>
    void test_structure()
    {
        using VD = typename Graph::vertex_descriptor;
        using ED = typename Graph::edge_descriptor;
    


        Graph g;
        REQUIRE(g.num_vertices() == 0);
        REQUIRE(g.num_edges() == 0);
        REQUIRE(g.vertices().empty());
        REQUIRE(g.edges().empty());

        VD u = g.add_vertex();
        REQUIRE(g.num_vertices() == 1);
        VD v = g.add_vertex();
        REQUIRE(g.num_vertices() == 2);

        {
            std::list<VD> lst {u, v};
            REQUIRE(is_permutation(g.vertices(), gubg::make_range(lst)));
        }

        ED e = g.add_edge(u, v);
        REQUIRE(g.num_edges() == 1);
        REQUIRE(g.source(e) == u);
        REQUIRE(g.target(e) == v);
        REQUIRE(g.edges().front() == e);

        REQUIRE(g.out_degree(u) == 1);
        REQUIRE(g.out_edges(u).front() == e);

        {
            std::list<ED> lst { e };
            test_in_edges(g, v, gubg::make_range(lst), typename Graph::direction_tag());
        }

        {
            std::list<VD> lst {v };
            REQUIRE(is_permutation(g.adjacent_vertices(u), gubg::make_range(lst)));
            
        }
    }

    template <typename VT, typename OET, typename ET>
    void test(VT, OET, ET)
    {
        using no_label = gubg::graph::no_label;
        test_structure<gubg::graph::AdjacencyList<VT, OET, ET, no_label, no_label, gubg::graph::undirected>>();
        test_structure<gubg::graph::AdjacencyList<VT, OET, ET, no_label, no_label, gubg::graph::directed>>();
        test_structure<gubg::graph::AdjacencyList<VT, OET, ET, no_label, no_label, gubg::graph::bidirectional>>();
    }
}

TEST_CASE("adjacency graph", "[ut][graph]")
{

    using list_tag = gubg::graph::use_list;
    using vct_tag = gubg::graph::use_vector;

    test(list_tag(), list_tag(), list_tag());
    test(list_tag(), list_tag(), vct_tag());
    test(list_tag(), vct_tag(), list_tag());
    test(list_tag(), vct_tag(), vct_tag());
    test(vct_tag(), list_tag(), list_tag());
    test(vct_tag(), list_tag(), vct_tag());
    test(vct_tag(), vct_tag(), list_tag());
    test(vct_tag(), vct_tag(), vct_tag());
}
