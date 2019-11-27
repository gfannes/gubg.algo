#ifndef HEADER_gubg_gp_tree_test_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_test_Node_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Node.hpp>
#include <cassert>

namespace gubg { namespace gp { namespace tree { namespace test { 

    using T = double;

    struct X
    {
        std::shared_ptr<T> ptr;

        X(): ptr(new double{0}) {}

        void set(T v)
        {
            assert(!!ptr);
            *ptr = v;
        }

        bool compute(T &v) const
        {
            assert(!!ptr);
            v = *ptr;
            return true;
        }
    };

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

} } } } 

#endif
