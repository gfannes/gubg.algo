#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>
#include <optional>

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

        template <typename Ftor>
        std::optional<std::size_t> find_child_ix(Ftor &&ftor) const
        {
            std::size_t ix = 0;
            for (const auto &node: childs.nodes)
            {
                if (ftor(node))
                    return ix;
                ++ix;
            }
            return std::optional<std::size_t>{};
        }

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
