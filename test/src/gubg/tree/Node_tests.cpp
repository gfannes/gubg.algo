#include <gubg/tree/Node.hpp>
#include <gubg/tree/print.hpp>
#include <catch.hpp>
#include <iostream>
using namespace gubg;

namespace  { 
    struct A { };

    class Visitor
    {
    public:
        void operator()(tree::NoData &)
        {
            std::cout << "NoData" << std::endl;
        }
        void operator()(A &a)
        {
            std::cout << "A" << std::endl;
        }
    private:
    };
} 

TEST_CASE("tree::Node creation test tests", "[tree][Node]")
{
    using Node = gubg::tree::Node<Visitor>;

    Node::Ptr n;
    size_t new_size = 0;
    bool can_resize = true;
    SECTION("leaf NoData")
    {
        n = tree::create_leaf<Node>();
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = false; }
    }
    SECTION("leaf A")
    {
        n = tree::create_leaf<Node>(A{});
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = false; }
    }
    SECTION("branch A, 0 childs")
    {
        n = tree::create_branch<Node>(A{});
        SECTION("resize 0") { }
        SECTION("resize 1") { new_size = 1; can_resize = true; }
    }
    SECTION("branch A, 2 childs")
    {
        n = tree::create_branch<Node>(A{}, 2);
        SECTION("resize 0") { }
        SECTION("resize 3") { new_size = 3; can_resize = true; }
    }

    Visitor v;
    n->visit(v);
    REQUIRE(n->childs().size() == n->size());
    const auto resize_ok = n->resize(new_size);
    REQUIRE(resize_ok == can_resize);
    REQUIRE(n->childs().size() == n->size());
    if (resize_ok)
        REQUIRE(n->size() == new_size);

    auto lambda = [](auto &os, auto &node){os << node.get();};
    tree::print(std::cout, n, lambda);
}
