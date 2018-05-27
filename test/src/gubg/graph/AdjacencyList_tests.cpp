#include "catch.hpp"
#include "gubg/graph/AdjacencyList.hpp"
#include "gubg/graph/Traits.hpp"
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

    template <typename Graph, typename VD, typename It> void test_in_edges(const Graph & g, VD v, const gubg::Range<It> & in_range, gubg::graph::undirected)
    {
        REQUIRE(is_permutation(in_edges(v,g), in_range));
        typename gubg::graph::Traits<Graph>::degree_type d = std::distance(RANGE(in_range));
        REQUIRE(in_degree(v, g) == d);

    }

    template <typename Graph, typename VD, typename It> void test_in_edges(const Graph & g, VD v, const gubg::Range<It> & in_range, gubg::graph::bidirectional)
    {
        REQUIRE(is_permutation(in_edges(v,g), in_range));
        typename Graph::degree_type d = std::distance(RANGE(in_range));
        REQUIRE(in_degree(v, g) == d);
    }


    template <typename Graph>
    void test_structure()
    {
        using Traits = gubg::graph::Traits<Graph>;
        using VD = typename Traits::vertex_descriptor;
        using ED = typename Traits::edge_descriptor;

        Graph g;
        REQUIRE(num_vertices(g) == 0);
        REQUIRE(num_edges(g) == 0);
        REQUIRE(vertices(g).empty());
        REQUIRE(edges(g).empty());

        VD u = add_vertex(g);
        REQUIRE(num_vertices(g) == 1);
        VD v = add_vertex(g);
        REQUIRE(num_vertices(g) == 2);

        {
            std::list<VD> lst {u, v};
            REQUIRE(is_permutation(vertices(g), gubg::make_range(lst)));
        }

        ED e = add_edge(u, v, g);
        REQUIRE(num_edges(g) == 1);
        REQUIRE(source(e, g) == u);
        REQUIRE(target(e, g) == v);
        REQUIRE(edges(g).front() == e);

        REQUIRE(out_degree(u, g) == 1);
        REQUIRE(out_edges(u, g).front() == e);

        {
            std::list<ED> lst { e };
            test_in_edges(g, v, gubg::make_range(lst), typename Traits::direction_tag());
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
//        test_structure<gubg::graph::AdjacencyList<VT, OET, ET, no_label, no_label, gubg::graph::directed>>();
//        test_structure<gubg::graph::AdjacencyList<VT, OET, ET, no_label, no_label, gubg::graph::bidirectional>>();
    }
    
    using list_tag = gubg::graph::use_list;
    using vct_tag = gubg::graph::use_vector;
}

TEST_CASE("adjacency graph", "[ut][graph]")
{

    test(list_tag(), list_tag(), list_tag());
//    test(list_tag(), list_tag(), vct_tag());
//    test(list_tag(), vct_tag(), list_tag());
//    test(list_tag(), vct_tag(), vct_tag());
//    test(vct_tag(), list_tag(), list_tag());
//    test(vct_tag(), list_tag(), vct_tag());
//    test(vct_tag(), vct_tag(), list_tag());
//    test(vct_tag(), vct_tag(), vct_tag());
}

TEST_CASE("adjacency graph label", "[ut][graph]")
{
    struct EdgeType 
    {  
        EdgeType(int i)
        : v(i)
        {
        }

        int v;
    };

    gubg::graph::AdjacencyList<list_tag, list_tag, list_tag, std::string, EdgeType> g;

    auto u = add_vertex(g);
    auto v = add_vertex("test", g);

    REQUIRE(vertex_label(u, g) == "");
    REQUIRE(vertex_label(v, g) == "test");

    auto e = add_edge(u, v, EdgeType(2), g);
    REQUIRE(edge_label(e, g).v == 2);
    edge_label(e, g).v = 3;
    REQUIRE(edge_label(e, g).v == 3);

}
