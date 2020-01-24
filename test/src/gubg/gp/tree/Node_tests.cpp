#include "./test/Node.hpp"
#include <catch.hpp>
#include <memory>
using namespace gubg::gp::tree;

TEST_CASE("gp::tree::Node tests", "[ut][gp][tree][Node]")
{
    using X = test::X;
    using Plus = test::Plus;
    using Node = test::Node;

    struct Exp
    {
    };
    Exp exp;

    X x;
    SECTION("x nullptr") { }
    SECTION("x == 0") { x.ptr.reset(new double{0}); }
    SECTION("x == 42") { x.ptr.reset(new double{42}); }

    auto x_t = create_terminal<Node>(x);
    REQUIRE(!!x_t);

    Plus plus;
    auto plus_f = create_function<Node>(plus);
    REQUIRE(!!plus_f);
    auto childs = plus_f->childs();
    childs[0] = x_t;
    childs[1] = x_t;
}
