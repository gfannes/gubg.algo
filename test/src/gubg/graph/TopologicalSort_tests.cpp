#include "catch.hpp"
#include "gubg/graph/TopologicalSort.hpp"
#include "gubg/graph/AdjacencyList.hpp"

namespace {

    template <typename It, typename Graph>
    bool validate_order(const gubg::Range<It> & order, const Graph & g)
    {
        MSS_BEGIN(bool);

        typename gubg::graph::Traits<Graph>::vertices_size_type vd;

        // every vertex occurs exactly once
        for(auto v : vertices(g))
            MSS(std::count(RANGE(order), v) == 1);

        MSS(order.size() == num_vertices(g));

        for(auto it = order.begin(); it != order.end(); ++it)
        {
            auto u = *it;
            for(auto v : adjacent_vertices(u, g))
                MSS(std::find(RANGE(order), v) > it);
        }

        for(auto v : vertices(g))
            MSS(std::count(RANGE(order), v) == 1);
        
        MSS_END();
    }

    template <typename G>
    void construct_1(G & g, bool & is_a_dag)
    {
        auto prev = g.add_vertex();
        for(unsigned int i = 0; i < 9; ++i)
        {
            auto nxt = add_vertex(g);
            add_edge(prev, nxt, g);
            prev = nxt;
        }

        is_a_dag = true;
    }
    
    template <typename G>
    void construct_2(G & g, bool & is_a_dag)
    {
        construct_1(g, is_a_dag);
        add_edge(g.vertices().back(), g.vertices().front(), g);
        is_a_dag = false;
    }

    template <typename G>
    void perform_test()
    {
        bool is_dag = true;
        G g;

        SECTION("empty") {}
        SECTION("g1") { construct_1(g, is_dag); }
        SECTION("g2") { construct_2(g, is_dag); }

        std::vector<typename gubg::graph::Traits<G>::vertex_descriptor> vertices(g.num_vertices());
        bool dag = gubg::graph::construct_topological_order(g, vertices.begin());

        REQUIRE(dag == is_dag);

        if(is_dag)
            REQUIRE(validate_order(gubg::make_range(vertices), g));
    }

}


TEST_CASE("Topological sort test", "[ut][graph][topological_order]")
{
    using use_list = gubg::graph::use_list;
    using no_label = gubg::graph::no_label;

    perform_test<gubg::graph::AdjacencyList<use_list, use_list, use_list, no_label, no_label, gubg::graph::bidirectional>>();
    perform_test<gubg::graph::AdjacencyList<use_list, use_list, use_list, no_label, no_label, gubg::graph::directed>>();
}
