#include "./test/Node.hpp"
#include <gubg/gp/tree/Grow.hpp>
#include <catch.hpp>
#include <vector>
#include <random>
using namespace gubg::gp;

TEST_CASE("gp::tree::grow tests", "[ut][gp][tree][Grow]")
{
    using T = tree::test::T;
    using Node = tree::Node<T, tree::test::Base>;
    using NodePtr = typename Node::Ptr;

    std::mt19937 rng;

    std::vector<NodePtr> terminals;
    tree::test::X x;
    terminals.push_back(tree::create_terminal<Node>(x));
    auto terminal_factory = [&](auto &ptr)
    {
        ptr = terminals[std::uniform_int_distribution<>{0,terminals.size()-1}(rng)]->clone();
        return true;
    };

    std::vector<NodePtr> functions;
    tree::test::Plus plus;
    functions.push_back(tree::create_function<Node>(plus));
    auto function_factory = [&](auto &ptr)
    {
        ptr = functions[std::uniform_int_distribution<>{0,functions.size()-1}(rng)]->clone();
        return true;
    };

    tree::Grow<Node> grow;
    REQUIRE(grow.set_probs(1,1));
    REQUIRE(grow.set_max_depth(3));

    std::vector<NodePtr> trees(100);
    for (auto &tree: trees)
    {
        REQUIRE(grow(tree, terminal_factory, function_factory));
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
