#include "catch.hpp"
#include "gubg/optimization/SCG.hpp"
#include <vector>
#include <cassert>
using namespace gubg;

namespace  { 
    using Float = double;

    struct Params
    {
        using Type = std::vector<Float>;
        static Float sum_squares(const Type &params)
        {
            Float ss = 0.0;
            for (auto v: params)
                ss += v*v;
            return ss;
        }
    };
    struct Function
    {
        Float operator()(const Params::Type &params) const
        {
            return Params::sum_squares(params)/2.0;
        }
    };
    struct Gradient
    {
        void operator()(Params::Type &gradient, const Params::Type &params) const
        {
            assert(gradient.size() == params.size());
            gradient = params;
        }
    };
} 

TEST_CASE("optimization::SCG tests without outer", "[ut][scg]")
{
    optimization::SCG<Float, Params> scg;

    Params::Type params;
    Function f;
    Gradient g;

    scg(params, f, g);
}

namespace  { 
    struct Outer
    {
    };
} 
TEST_CASE("optimization::SCG tests with outer", "[ut][scg]")
{
    Outer outer;
    optimization::SCG<Float, Params, Outer> scg(outer);

    Params::Type params;
    Function f;
    Gradient g;

    scg(params, f, g);
}
