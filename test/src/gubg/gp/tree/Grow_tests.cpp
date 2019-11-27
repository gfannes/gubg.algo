#include "./test/Node.hpp"
#include <gubg/gp/tree/Grow.hpp>
#include <catch.hpp>
#include <vector>
using namespace gubg::gp::tree;

TEST_CASE("gp::tree::grow tests", "[ut][gp][tree][Grow]")
{
    using T = test::T;
    using NodePtr = typename Node<T>::Ptr;

    std::vector<NodePtr> terminals, functions;
    test::X x;
    terminals.push_back(create_terminal<T>(x));
    test::Plus plus;
    functions.push_back(create_function<T>(plus));

    NodePtr root;
    {
        Grow<T> grow;
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_probs(0.1,1));
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_max_depth(3));
        REQUIRE(grow(root, terminals, functions));
    }
}
