#ifndef HEADER_gubg_tree_print_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_print_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace tree { 

    template <typename NodePtr, typename Ftor>
    bool print(std::ostream &os, NodePtr &root, Ftor &&ftor)
    {
        MSS_BEGIN(bool);
        auto my_ftor = [&](auto &node, const auto &path, bool enter)
        {
            /* if (enter) */
            {
                os << std::string(path.size()*2, ' ');
                os << (enter ? ">>" : "<<");
                ftor(os, node);
                os << std::endl;
            }
            return true;
        };
        Path path;
        MSS(dfs(root, my_ftor, path));
        MSS_END();
    }

} } 

#endif
