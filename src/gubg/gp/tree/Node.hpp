#ifndef HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace gubg { namespace gp { namespace tree { 

    //Path into a tree
    using Path = std::vector<unsigned int>;

    //Abstract base class for Terminals and Functions
    //Base_ is the base type for Terminal values and Function operations
    //Via the Node.base() method, you can get to this base
    //Node, Terminal and Function are responsible for the tree structure
    //If you need access to Node from Base, you have to pass it explicitly
    template <typename Base_>
    class Node
    {
    public:
        using Base = Base_;
        using Self = Node<Base>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;
        using Childs_const = Range<const Ptr*>;

        std::size_t size() const
        {
            std::size_t nr = 0;
            size_recursive_(nr);
            return nr;
        }

        virtual ~Node() {}
        virtual Ptr clone(bool deep) = 0;
        virtual Base &base() = 0;
        virtual Childs childs() = 0;
        virtual Childs_const childs_const() const = 0;

    private:
        void size_recursive_(std::size_t &nr) const
        {
            ++nr;
            for (auto child_ptr: childs_const())
                child_ptr->size_recursive_(nr);
        }
    };

    template <typename NodePtr, typename Ftor>
    bool dfs(NodePtr &root, Ftor &&ftor, Path &path)
    {
        MSS_BEGIN(bool);
        MSS(!!root);
        MSS(ftor(root, path, true));
        path.push_back(0);
        for (auto &child: root->childs())
        {
            MSS(dfs(child, ftor, path));
            ++path.back();
        }
        path.pop_back();
        MSS(ftor(root, path, false));
        MSS_END();
    }

    template <typename NodePtr>
    NodePtr *find(NodePtr &root, const Path &path, std::size_t offset = 0)
    {
        if (offset >= path.size())
            return &root;
        const auto chix = path[offset];
        auto chs = root->childs();
        if (chix >= chs.size())
            return nullptr;
        return find(chs[chix], path, offset+1);
    }

    //TODO: make this const
    template <typename NodePtr>
    std::vector<Path> all_paths(NodePtr &root)
    {
        std::vector<Path> paths;
        auto append_path = [&](const auto &root, const auto &path, bool is_enter)
        {
            if (is_enter)
                paths.push_back(path);
            return true;
        };
        Path path;
        const auto ok = dfs(root, append_path, path);
        assert(ok);
        return paths;
    }

} } } 

#endif
