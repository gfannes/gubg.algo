#ifndef HEADER_gubg_tree_stream_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_stream_hpp_ALREADY_INCLUDED

#include <gubg/hr.hpp>
#include <ostream>

namespace gubg { namespace tree { 

    template <typename Forest, typename Ftor>
    void stream(std::ostream &os, const Forest &forest, Ftor &&ftor)
    {
        auto my_ftor = [&](const auto &node, const auto &path, unsigned int count)
        {
            if (count == 0)
            {
                os << std::string((path.size()-1)*2, ' ');
                /* os << (count == 0 ? ">>" : "<<"); */
                os << (count == 0 ? "# " : "  ");
                ftor(os, node);
                os << " " << gubg::hr(path);
                os << std::endl;
            }
        };
        forest.dfs(my_ftor);
    }

    template <typename Forest>
    void stream(std::ostream &os, const Forest &forest)
    {
        stream(os, forest, [](std::ostream &os, const auto &node){os << node.value;});
    }

} } 

#endif
