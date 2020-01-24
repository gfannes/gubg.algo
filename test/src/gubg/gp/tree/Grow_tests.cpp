#include "./test/Node.hpp"
#include <gubg/gp/tree/Grow.hpp>
#include <catch.hpp>
#include <vector>
#include <random>
using namespace gubg::gp;

TEST_CASE("gp::tree::grow tests", "[ut][gp][tree][Grow]")
{
    using Node = tree::Node<tree::test::Base>;
    using NodePtr = typename Node::Ptr;

    std::mt19937 rng;

    std::vector<NodePtr> terminals;
    tree::test::X x;
    terminals.push_back(tree::create_terminal<Node>(x));
    auto terminal_factory = [&](auto &ptr)
    {
        ptr = terminals[std::uniform_int_distribution<>(0,terminals.size()-1)(rng)]->clone(true);
        return true;
    };

    std::vector<NodePtr> functions;
    tree::test::Plus plus;
    functions.push_back(tree::create_function<Node>(plus));
    auto function_factory = [&](auto &ptr)
    {
        ptr = functions[std::uniform_int_distribution<>(0,functions.size()-1)(rng)]->clone(true);
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

    auto print = [&](auto n, const auto &path, bool is_enter)
    {
        std::cout << std::string(2*path.size(), ' ') << n->base().hr() << std::endl;
        return true;
    };
    for (auto &tree: trees)
    {
        std::cout << tree.get() << std::endl;
        tree::Path path;
        tree::dfs(tree, print, path);
    }
}
