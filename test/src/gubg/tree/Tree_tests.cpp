#include <gubg/tree/Forest.hpp>
#include <gubg/tree/stream.hpp>
#include <gubg/hr.hpp>
#include <catch.hpp>
#include <iostream>
#include <list>
using namespace gubg;

namespace  { 
    struct Data
    {
        std::string name;
        Data() {}
        Data(const std::string &name): name(name) {}
    };
    std::ostream &operator<<(std::ostream &os, const Data &data)
    {
        os << data.name;
        return os;
    }
} 

TEST_CASE("tree tests", "[tree][Forest]")
{
    using Forest = gubg::tree::Forest<Data>;
    using Node = Forest::Node;
    using Path = gubg::tree::Path;

    Forest forest;
    REQUIRE(forest.empty());

    Path path;
    REQUIRE(!forest.find(path));
    REQUIRE(!forest.find(path, false));
    REQUIRE(!forest.find(0));
    REQUIRE(!forest.find(0, false));

    {
        path = Path{3,2,1,0};
        REQUIRE(!forest.find(path));
        REQUIRE(!!forest.find(path, true));
        REQUIRE(!forest.empty());
        REQUIRE(!!forest.find(0));
        REQUIRE(!!forest.find(1));
        REQUIRE(!!forest.find(2));
        REQUIRE(!!forest.find(3));
        REQUIRE(!forest.find(4));
        auto n = forest.find(path);
        REQUIRE(!!n);
        REQUIRE(n->is_leaf());
        n->value.name = "3210";

        n = forest.find(3);
        REQUIRE(!n->is_leaf());
    }

    {
        path = Path{0,1,2,3};
        auto n = forest.find(path, true);
        REQUIRE(!!n);
        REQUIRE(n->is_leaf());
        n->value.name = "0123";
    }

    REQUIRE(forest.node_count() == 19);

    auto print = [&](const auto &f)
    {
        std::cout << "Forest " << &f << std::endl;
        auto indent = [&](const auto &path) { return std::string(path.size()*2, ' '); };
        auto ftor = [&](const auto &node, const auto &path, unsigned int count)
        {
            std::cout << indent(path);
            if (count == 0)
                std::cout << ">> ";
            else
                std::cout << "<< ";
            std::cout << &node <<  " " << gubg::hr(path) << " " << node.value.name << std::endl;
        };
        f.dfs(ftor);
        std::cout << std::endl;
    };
    print(forest);

    auto forest_copy = forest;
    REQUIRE(forest_copy.node_count() == forest.node_count());
    print(forest_copy);

    tree::stream(std::cout, forest);
    tree::stream(std::cout, forest, [](std::ostream &os, const Node &node){os << node.value.name.size();});

    {
        auto a = forest.find(Path{0});
        REQUIRE(!!a);
        auto b = forest.find(Path{3});
        REQUIRE(!!b);
        std::swap(*a, *b);
        print(forest);
    }

    std::list<Forest> wood;
    for (auto ix = 0u; ix < 100000; ++ix)
        wood.push_back(forest);
}
