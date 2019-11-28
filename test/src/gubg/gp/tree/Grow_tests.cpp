#include "./test/Node.hpp"
#include <gubg/gp/tree/Grow.hpp>
#include <catch.hpp>
#include <vector>
using namespace gubg::gp;

TEST_CASE("gp::tree::grow tests", "[ut][gp][tree][Grow]")
{
    using T = tree::test::T;
    using Node = tree::Node<T, tree::test::Base>;
    using NodePtr = typename Node::Ptr;

    std::vector<NodePtr> terminals, functions;
    tree::test::X x;
    terminals.push_back(tree::create_terminal<Node>(x));
    tree::test::Plus plus;
    functions.push_back(tree::create_function<Node>(plus));

    NodePtr root;
    {
        tree::Grow<Node> grow;
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_probs(0.1,1));
        REQUIRE(!grow(root, terminals, functions));
        REQUIRE(grow.set_max_depth(3));
        REQUIRE(grow(root, terminals, functions));
    }

    int level = -1;
    typename Node::Functor print = [&](const Node::Base *base, tree::DFS dfs)
    {
        level += (dfs == tree::DFS::Open);
        std::cout << std::string(2*level, ' ') << base->hr() << std::endl;
        level -= (dfs == tree::DFS::Close);
    };
    root->dfs(print);
}
