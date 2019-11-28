#include "./test/Node.hpp"
#include <gubg/gp/tree/Grow.hpp>
#include <catch.hpp>
#include <vector>
using namespace gubg::gp;

TEST_CASE("gp::tree::grow tests", "[ut][gp][tree][Grow]")
{
    using T = tree::test::T;
    using Node = tree::Node<T>;
    using NodePtr = typename Node::Ptr;

    std::vector<NodePtr> terminals, functions;
    tree::test::X x;
    terminals.push_back(tree::create_terminal<T>(x));
    tree::test::Plus plus;
    functions.push_back(tree::create_function<T>(plus));

    NodePtr root;
    {
        tree::Grow<T> grow;
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_probs(0.1,1));
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_max_depth(3));
        REQUIRE(grow(root, terminals, functions));
    }

    int level = -1;
    typename Node::Functor print = [&](const Node *node, tree::DFS dfs)
    {
        level += (dfs == tree::DFS::Open);
        std::cout << std::string(2*level, ' ') << node << std::endl;
        level -= (dfs == tree::DFS::Close);
    };
    root->dfs(print);
}
