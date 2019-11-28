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

    tree::Grow<Node> grow;
    REQUIRE(grow.set_probs(1,1));
    REQUIRE(grow.set_max_depth(3));

    std::vector<NodePtr> trees(100);
    for (auto &tree: trees)
    {
        REQUIRE(grow(tree, terminals, functions));
    }
    std::cout << "All trees are grown" << std::endl;

    int level = 0;
    typename Node::Functor print = [&](const Node::Base *base, tree::DFS dfs)
    {
        level -= (dfs == tree::DFS::Close);
        std::cout << std::string(2*level, ' ') << base->hr() << std::endl;
        level += (dfs == tree::DFS::Open);
    };
    for (auto &tree: trees)
    {
        std::cout << tree.get() << std::endl;
        tree->dfs(print);
    }
}
