#include <gubg/tree/grow.hpp>
#include <gubg/prob/Bernoulli.hpp>
#include <gubg/tree/stream.hpp>
#include <gubg/hr.hpp>
#include <catch.hpp>
#include <string>
#include <iostream>

TEST_CASE("tree::grow tests", "[tree][grow]")
{
    using Forest = gubg::tree::Forest<std::string>;

    Forest forest;

    auto create_node = [&](auto &node, const auto &path)
    {
        node.value = gubg::hr(path);
        node.childs.nodes.resize(gubg::prob::choose(2, 0, 3.0/path.size(), true));
    };
    gubg::tree::grow(forest, create_node);

    gubg::tree::stream(std::cout, forest);
}
