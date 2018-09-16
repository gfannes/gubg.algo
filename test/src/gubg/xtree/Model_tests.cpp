#include "catch.hpp"
#include "gubg/xtree/Model.hpp"
#include <iostream>

namespace  { 
    struct Data
    {
        std::string name;
        int v = 0;
        Data(){}
        Data(const std::string &name, int v): name(name), v(v) {}
    };
} 

TEST_CASE("gubg::Xtree tests", "[ut][xtree][Model]")
{
    using Model = gubg::xtree::Model<Data>;
    Model xtree;
    REQUIRE(xtree.root().v == 0);
    {
        auto &n = xtree.root().emplace_back("a", 42);
        REQUIRE(n.v == 42);
    }
    xtree.root().emplace_back("b", 1);

    const auto acc = xtree.accumulate(0, [](int acc, const auto &node){return acc + node.v;});
    REQUIRE(acc == 43);

    auto lambda = [](bool, const auto &node){
        Model::Path path;
        node.path(path);
        for (const auto &ptr: path)
            std::cout << ptr->name << '/';
        std::cout << std::endl;
        std::cout << node.v << std::endl;
        return true;
    };
    xtree.accumulate(true, lambda);
}
