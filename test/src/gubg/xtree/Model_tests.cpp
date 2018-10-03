#include "catch.hpp"
#include "gubg/xtree/Model.hpp"
#include <iostream>
#include <sstream>

namespace  { 
    struct Data
    {
        std::string name;
        int v = 0;
        Data(){}
        Data(const std::string &name): name(name) {}
        Data(const std::string &name, int v): name(name), v(v) {}
    };
    using Model = gubg::xtree::Model<Data>;

    auto print_node = [](bool, const auto &node){
        std::ostringstream oss;
        node.each_out([&](const auto &to){oss << " ->" << to.name;});
        node.each_in([&](const auto &to){oss << " <-" << to.name;});
        node.each_sub([&](const auto &to){oss << " =>" << to.name;});

        Model::Path path;
        node.path(path);
        for (const auto &ptr: path)
            std::cout << ptr->name << '/';
        std::cout << ": " << node.v << oss.str() << std::endl;
        return true;
    };
} 

TEST_CASE("gubg::xtree tests", "[ut][xtree][Model]")
{
    Model xtree;
    REQUIRE(xtree.root().v == 0);
    {
        auto &n = xtree.root().emplace_back("a", 42);
        REQUIRE(n.v == 42);
    }
    xtree.root().emplace_back("b", 1);

    const auto acc = xtree.accumulate(0, [](int acc, const auto &node){return acc + node.v;});
    REQUIRE(acc == 43);

    xtree.accumulate(true, print_node);
}

TEST_CASE("gubg::xtree xlinks test", "[ut][xtree][xlinks]")
{
    Model xtree;

    auto &root = xtree.root();
    auto &a = root.emplace_back("a");
    auto &b = a.emplace_back("b");
    auto &c = a.emplace_back("c");
    auto &d = b.emplace_back("d");
    auto &e = b.emplace_back("e");
    auto &f = c.emplace_back("f");
    auto &g = c.emplace_back("g");

    f.add_link(b);
    d.add_link(c);

    xtree.process_xlinks();

    xtree.accumulate(true, print_node);
}
