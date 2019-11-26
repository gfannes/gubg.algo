#include <gubg/gp/Tree.hpp>
#include <catch.hpp>

namespace  { 
    using T = double;

    struct X
    {
        T v = 0;
        bool compute(T &v) const
        {
            v = this->v;
            return true;
        }
    };
    using X_t = gubg::gp::Terminal<T, X>;

    struct Plus
    {
        std::size_t size() const { return 2; }
        bool compute(T &v, const T *begin, const T *end) const
        {
            v = 0;
            for (auto it = begin; it < end; ++it)
                v += *it;
            return true;
        }
    };
    using Plus_f = gubg::gp::Function<T, Plus>;
} 

TEST_CASE("gp::Tree tests", "[ut][gp][Tree]")
{
    X x;
    auto x_t = gubg::gp::create_terminal<T>(x);
    SECTION("x == 0") { x.v = 0; }
    SECTION("x == 42") { x.v = 42; }

    T v = 1;
    REQUIRE(x_t->compute(v));
    REQUIRE(v == x.v);

    Plus plus;
    auto plus_f = gubg::gp::create_function<T>(plus);
    auto childs = plus_f->childs();
    childs[0] = x_t;
    childs[1] = x_t;
    REQUIRE(plus_f->compute(v));
    REQUIRE(v == (x.v+x.v));
}
