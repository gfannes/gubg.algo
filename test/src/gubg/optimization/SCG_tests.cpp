#include "catch.hpp"
#include "gubg/optimization/SCG.hpp"
#include <vector>
#include <cassert>
#include <iostream>
using namespace gubg;

namespace  { 
    using Float = double;

    struct Params
    {
        using Type = std::vector<Float>;
        static unsigned int order(const Type &params)
        {
            return params.size();
        }
        static Float sum_squares(const Type &params)
        {
            Float ss = 0.0;
            for (auto v: params)
                ss += v*v;
            return ss;
        }
        static Float inprod(const Type &a, const Type &b)
        {
            assert(a.size() == b.size());
            Float res = 0.0;
            const auto size = a.size();
            for (size_t ix = 0; ix < size; ++ix)
                res += a[ix]*b[ix];
            return res;
        }
        static void update(Type &dst, Float factor, const Type &src)
        {
            auto it = dst.begin();
            for (auto v: src)
                *it++ += factor*v;
        }
    };
    void print(const Params::Type &params)
    {
        std::cout << "[";
        for (auto v: params)
            std::cout << v << " ";
        std::cout << "]";
    }

    struct Function
    {
        Float operator()(const Params::Type &params) const
        {
            const Float v = -Params::sum_squares(params);
            std::cout << "Function in "; print(params); std::cout << " is " << v << std::endl;
            return v;
        }
    };
    struct Gradient
    {
        void operator()(Params::Type &gradient, const Params::Type &params) const
        {
            assert(gradient.size() == params.size());
            gradient = params;
            for (auto &v: gradient)
                v = -2.0*v;
            std::cout << "Gradient in "; print(params); std::cout << " is "; print(gradient); std::cout << std::endl;
        }
    };
} 

TEST_CASE("optimization::SCG tests without outer", "[ut][scg_]")
{
    optimization::SCG<Float, Params> scg;

    Params::Type params = {1.0, 2.0};
    Function f;
    Gradient g;

    scg(params, f, g);
}

namespace  { 
    struct Outer
    {
        void scg_params(unsigned int iteration, Float eval, const Params::Type &params)
        {
            std::cout << std::endl << "Iteration " << iteration << ": " << eval << " @ ";
            print(params);
            std::cout << std::endl;
        }
        bool scg_terminate(unsigned int iteration, Float eval, const Params::Type &gradient)
        {
            const auto ss_gradient = Params::sum_squares(gradient);
            std::cout << "Gradient: "; print(gradient); std::cout << ", ss: " << ss_gradient << std::endl;
            return ss_gradient <= 0.0001;
        }
    };
} 
TEST_CASE("optimization::SCG tests with outer", "[ut][optimization][SCG]")
{
    Outer outer;
    optimization::SCG<Float, Params, Outer> scg(outer);

    Params::Type params = {1.0, 2.0};
    Function f;
    Gradient g;

    scg(params, f, g);
}
