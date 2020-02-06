#ifndef HEADER_gubg_tree_grow_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_grow_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>

namespace gubg { namespace tree { 

    namespace details { 
        template <typename Node, typename Ftor>
        void grow(Node &node, Path &path, Ftor &&ftor)
        {
            ftor(node, path);
            path.emplace_back(0);
            auto &child_nodes = node.childs.nodes;
            const auto size = child_nodes.size();
            for (auto nix = 0u; nix < size; ++nix)
            {
                path.back() = nix;
                grow(child_nodes[nix], path, ftor);
            }
            path.pop_back();
        }
    } 

    //ftor is called on newly created nodes together with their path and can hence specialize it: ftor(node, path)
    //The first node in the forest is created automatically, the rest of the childs sholud be created via node.childs.nodes.resize()
    template <typename Forest, typename Ftor>
    void grow(Forest &forest, Ftor &&ftor)
    {
        forest.nodes.clear();
        forest.nodes.emplace_back();
        Path path{0};
        details::grow(forest.nodes[0], path, ftor);
    }

} } 

#endif
