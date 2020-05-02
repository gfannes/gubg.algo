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
        std::size_t nr_childs() const {return childs.size();}

        //Depth-first search. ftor is called with arguments: ftor(node, path, visit_count)
        template <typename Ftor>
        void dfs(Ftor &&ftor, Path &path)
        {
            const auto size = childs.nodes.size();
            auto nix = 0u;
            for (; nix < size; ++nix)
            {
                ftor(*this, path, nix);
                path.push_back(nix);
                childs.nodes[nix].dfs(ftor, path);
                path.pop_back();
            }
            ftor(*this, path, nix);
        }
        template <typename Ftor>
        void dfs(Ftor &&ftor, Path &path) const
        {
            const auto size = childs.nodes.size();
            auto nix = 0u;
            for (; nix < size; ++nix)
            {
                ftor(*this, path, nix);
                path.push_back(nix);
                childs.nodes[nix].dfs(ftor, path);
                path.pop_back();
            }
            ftor(*this, path, nix);
        }

    private:
    };

} } 

#endif
