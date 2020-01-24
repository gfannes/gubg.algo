#ifndef HEADER_gubg_gp_tree_test_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_test_Node_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Terminal.hpp>
#include <gubg/gp/tree/Function.hpp>
#include <cassert>

namespace gubg { namespace gp { namespace tree { namespace test { 

    struct Base
    {
        virtual std::string hr() const = 0;
    };

    struct X: Base
    {
        std::shared_ptr<double> ptr;

        X(): ptr(new double{0}) {}

        std::string hr() const override {return "X";}

        void set(double v) { assert(!!ptr); *ptr = v; }

        bool compute(double &v) const { assert(!!ptr); v = *ptr; return true; }
    };

    struct Plus: Base
    {
        std::string hr() const override {return "Plus";}

        std::size_t size() const { return 2; }
    };

    using Node = tree::Node<Base>;

} } } } 

#endif
