#ifndef HEADER_gubg_tree_Forest_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Forest_hpp_ALREADY_INCLUDED

#include <vector>
#include <cstdint>
#include <numeric>

namespace gubg { namespace tree { 

    //Path into a forest
    using Path = std::vector<std::size_t>;

    template <typename T>
    class Node;

    //Group of Nodes
    template <typename T>
    class Forest
    {
    public:
        using Self = Forest<T>;
        using Node = tree::Node<T>;
        using Nodes = std::vector<Node>;

        //We give public access to the nodes
        Nodes nodes;

        bool empty() const {return nodes.empty();}

        void clear() {*this = Self{};}

        std::size_t node_count() const { return std::accumulate(nodes.begin(), nodes.end(), std::size_t{0}, [](std::size_t count, const Node &node){return count+1+node.childs.node_count();}); }

        //Find/create a node based on a full path
        Node *find(const Path &path, bool create = false) { return find_(path, create, 0u); }
        const Node *find(const Path &path) const { return find_(path, 0u); }

        //Find/create a node based on an index
        Node *find(std::size_t nix, bool create = false) {return nix < nodes.size() ? &nodes[nix] : (create ? (nodes.resize(nix+1), &nodes[nix]) : nullptr);}
        const Node *find(std::size_t nix) const {return nix < nodes.size() ? &nodes[nix] : nullptr;}

        //Depth-first search. ftor is called with arguments: ftor(node, path, visit_count)
        template <typename Ftor>
        void dfs(Ftor &&ftor)
        {
            Path path = {0u};
            const auto size = nodes.size();
            for (auto nix = 0u; nix < size; ++nix)
            {
                path.back() = nix;
                nodes[nix].dfs(ftor, path);
            }
        }
        template <typename Ftor>
        void dfs(Ftor &&ftor) const
        {
            Path path = {0u};
            const auto size = nodes.size();
            for (auto nix = 0u; nix < size; ++nix)
            {
                path.back() = nix;
                nodes[nix].dfs(ftor, path);
            }
        }

    private:
        Node *find_(const Path &path, bool create, std::size_t offset)
        {
            if (path.size() <= offset)
                //Path is too short
                return nullptr;
            const auto nix = path[offset];
            if (nodes.size() <= nix)
            {
                if (!create)
                    //No such node and we cannot create it
                    return nullptr;
                nodes.resize(nix+1);
            }
            auto &node = nodes[nix];
            if (offset+1 == path.size())
                return &node;
            return node.childs.find_(path, create, offset+1);
        }
        const Node *find_(const Path &path, std::size_t offset) const
        {
            if (path.size() <= offset)
                //Path is too short
                return nullptr;
            const auto nix = path[offset];
            if (nodes.size() <= nix)
                //No such node and we cannot create it
                return nullptr;
            const auto &node = nodes[nix];
            if (offset+1 == path.size())
                return &node;
            return node.childs.find_(path, offset+1);
        }
    };

} } 

#include <gubg/tree/Node.hpp>

#endif
