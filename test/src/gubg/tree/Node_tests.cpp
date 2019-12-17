#include <gubg/tree/Node.hpp>
#include <gubg/tree/print.hpp>
#include <catch.hpp>
#include <iostream>
using namespace gubg;

namespace  { 
    struct Data
    {
        std::string name;
        Data() {}
        Data(const std::string &name): name(name) {}
    };

    class Visitor
    {
    public:
        virtual void operator()(tree::NoData &)
        {
            std::cout << "NoData" << std::endl;
        }
        virtual void operator()(Data &data)
        {
            std::cout << "Data:" << data.name << std::endl;
        }
    private:
    };
    class NameExtractor: public Visitor
    {
    public:
        std::string name;
        void operator()(tree::NoData &) override
        {
            name = "NoData";
        }
        void operator()(Data &data) override
        {
            name = "Data:";
            name += data.name;
        }
    };
} 

TEST_CASE("tree::Node creation test tests", "[tree][Node]")
{
    using Node = gubg::tree::Node<Visitor>;

    Node::Ptr n;
    size_t new_size = 0;
    bool can_resize = true;
    SECTION("empty tree")
    {
    }
    SECTION("leaf NoData")
    {
        n = tree::create_leaf<Node>();
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = false; }
    }
    SECTION("leaf A")
    {
        n = tree::create_leaf<Node>(Data{"A"});
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = false; }
    }
    SECTION("branch A, 0 childs")
    {
        n = tree::create_branch<Node>(Data{"A"});
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = true; }
    }
    SECTION("branch A, 2 childs")
    {
        n = tree::create_branch<Node>(Data{"A"}, 2);
        SECTION("resize 0") { }
        SECTION("resize 3") { new_size = 3; can_resize = true; }
    }
    SECTION("branch A, leaf B, leaf C")
    {
        n = tree::create_branch<Node>(Data{"A"}, 2);
        new_size = 2;
        auto childs = n->childs();
        childs[0] = tree::create_leaf<Node>(Data{"B"});
        childs[1] = tree::create_leaf<Node>(Data{"C"});
    }

    Visitor v;
    if (n)
    {
        n->visit(v);
        REQUIRE(n->childs().size() == n->size());
        const auto resize_ok = n->resize(new_size);
        REQUIRE(resize_ok == can_resize);
        REQUIRE(n->childs().size() == n->size());
        if (resize_ok)
            REQUIRE(n->size() == new_size);
    }

    auto print_node = [&](auto &os, auto &node)
    {
        os << node.get();
        if (node)
        {
            NameExtractor name_extractor;
            node->visit(name_extractor);
            os << " " << name_extractor.name;
        }
    };
    tree::print(std::cout, n, print_node);

    tree::Path path;
    Node::Ptr *nn = nullptr;
    REQUIRE(tree::search(nn, n, path));
    REQUIRE(!!nn);
    print_node(std::cout, *nn);

    path.push_back(1);
    if (tree::search(nn, n, path))
    {
        REQUIRE(!!nn);
        print_node(std::cout, *nn);
        //We replace this node with another
        *nn = tree::create_leaf<Node>(Data{"D"});
        tree::print(std::cout, n, print_node);
    }
}
