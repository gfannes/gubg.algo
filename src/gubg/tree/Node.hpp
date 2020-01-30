#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>

namespace gubg { namespace tree { 

    template <typename T>
    class Node
    {
    public:
        using Forest = tree::Forest<T>;

        //We give public access to the value and childs
        T value;
        Forest childs;

        bool is_leaf() const {return childs.empty();}

    private:
    };

} } 

#endif
