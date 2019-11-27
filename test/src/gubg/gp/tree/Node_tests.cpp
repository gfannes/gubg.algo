#include "./test/Node.hpp"
#include <catch.hpp>
#include <memory>
using namespace gubg::gp::tree;

TEST_CASE("gp::tree::Node tests", "[ut][gp][tree][Node]")
{
    using T = test::T;
    using X = test::X;
    using Plus = test::Plus;

    struct Exp
    {
        bool compute_ok = true;
    };
    Exp exp;

    X x;
    SECTION("x nullptr") { }
    SECTION("x == 0") { x.ptr.reset(new double{0}); }
    SECTION("x == 42") { x.ptr.reset(new double{42}); }

    auto x_t = create_terminal<T>(x);

    T v = 1;
    const auto compute_ok = x_t->compute(v);
    REQUIRE(compute_ok == exp.compute_ok);
    if (compute_ok)
    {
        REQUIRE(v == *x.ptr);

        Plus plus;
        auto plus_f = create_function<T>(plus);
        auto childs = plus_f->childs();
        childs[0] = x_t;
        childs[1] = x_t;
        REQUIRE(plus_f->compute(v));
        REQUIRE(v == (*x.ptr+*x.ptr));
    }
}
