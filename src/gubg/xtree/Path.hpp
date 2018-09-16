#ifndef HEADER_gubg_xtree_Path_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Path_hpp_ALREADY_INCLUDED

#include <vector>

namespace gubg { namespace xtree { 

    template <typename Data>
    class Node;

    template <typename Data>
    using Path = std::vector<typename Node<Data>::CPtr>;

} } 

#endif
