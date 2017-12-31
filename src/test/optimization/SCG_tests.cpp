#include "catch.hpp"
#include "gubg/optimization/SCG.hpp"
#include <vector>
using namespace gubg;

namespace  { 
    using Float = double;

    struct Param
    {
        using Type = std::vector<Float>;
    };
} 

TEST_CASE("optimization::SCG tests without outer", "[ut][scg]")
{
    optimization::SCG<Float, Param> scg;

    scg();
}

namespace  { 
    struct Outer
    {
    };
} 
TEST_CASE("optimization::SCG tests with outer", "[ut][scg]")
{
    Outer outer;
    optimization::SCG<Float, Param, Outer> scg(outer);

    scg();
}
