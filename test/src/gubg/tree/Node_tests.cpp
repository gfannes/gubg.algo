#include <gubg/tree/Node.hpp>
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

TEST_CASE("tree::Node tests", "[tree][Node]")
{
    using Node = gubg::tree::Node<Visitor>;

    Node::Ptr n;
    SECTION("leaf NoData")
    {
        n = tree::create_leaf<Node>();
    }
    SECTION("leaf A")
    {
        n = tree::create_leaf<Node>(A{});
    }
    SECTION("branch A")
    {
        n = tree::create_branch<Node>(2, A{});
    }

    Visitor v;
    n->visit(v);
    std::cout << n->childs().size() << std::endl;
}
