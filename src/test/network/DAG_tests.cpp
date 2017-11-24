#include "catch.hpp"
#include "gubg/network/DAG.hpp"

TEST_CASE("gubg::network::DAG tests", "[ut][dag]")
{
    using Vertex = std::string;
    using DAG = gubg::network::DAG<Vertex>;

    Vertex a = "a";
    Vertex b = "b";
    Vertex c = "c";

    DAG dag;

    SECTION("step-by-step construction")
    {
        REQUIRE(dag.size() == 0);

        REQUIRE(dag.add_vertex(&a));
        REQUIRE(dag.size() == 1);

        dag.clear();
        REQUIRE(dag.size() == 0);

        REQUIRE(dag.add_vertex(&a));
        REQUIRE(dag.size() == 1);

        REQUIRE(!dag.add_vertex(&a));
        REQUIRE(dag.size() == 1);

        REQUIRE(dag.add_vertex(&b));


        REQUIRE(!dag.add_edge(&a, &a));
        REQUIRE(dag.add_edge(&a, &b));
        REQUIRE(!dag.add_edge(&a, &b));
        REQUIRE(!dag.add_edge(&b, &a));
        REQUIRE(dag.add_edge(&a, &c));

        REQUIRE(!dag.add_vertex(&c));
        REQUIRE(!dag.add_edge(&a, &c));
        REQUIRE(dag.add_edge(&b, &c));

        {
            char ch = 'a';
            auto check = [&](Vertex &v){
                MSS_BEGIN(bool);
                MSS((v)[0] == ch);
                ++ch;
                MSS_END();
            };
            REQUIRE(dag.each_vertex<gubg::network::Direction::Forward>(check));
        }
        {
            char ch = 'c';
            auto check = [&](Vertex &v){
                MSS_BEGIN(bool);
                MSS((v)[0] == ch);
                --ch;
                MSS_END();
            };
            REQUIRE(dag.each_vertex<gubg::network::Direction::Backward>(check));
        }

        REQUIRE(dag.remove_unreachables(&b));
        REQUIRE(dag.size() == 2);
        {
            char ch = 'b';
            auto check = [&](Vertex &v){
                MSS_BEGIN(bool);
                MSS((v)[0] == ch);
                ++ch;
                MSS_END();
            };
            REQUIRE(dag.each_vertex<gubg::network::Direction::Forward>(check));
        }
    }

    SECTION("proper reordering")
    {
        auto print = [&](){
            dag.stream(std::cout, [](const auto &n){return n;});
        };
        REQUIRE(dag.add_vertex(&c));
        REQUIRE(dag.add_vertex(&b));
        REQUIRE(dag.add_vertex(&a));
        print();
        REQUIRE(dag.add_edge(&a, &b));
        print();
        REQUIRE(dag.add_edge(&c, &a));
        print();
    }
}
