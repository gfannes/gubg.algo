#ifndef HEADER_gubg_xtree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Node_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Path.hpp"
#include <vector>
#include <memory>

namespace gubg { namespace xtree { 

    template <typename Data>
    class Model;

    template <typename Data>
    class Node: public Data, public std::enable_shared_from_this<Node<Data>>
    {
    private:
        using Self = Node<Data>;
        using SFT = std::enable_shared_from_this<Node<Data>>;

    public:
        using Ptr = std::shared_ptr<Self>;
        using CPtr = std::shared_ptr<const Self>;
        using WPtr = std::weak_ptr<Self>;
        using Path = xtree::Path<Data>;

        void path(Path &p) const
        {
            p.clear();
            path_(p, SFT::shared_from_this());
        }

        template <typename ... Args>
        Self &emplace_back(Args ... args)
        {
            Ptr child(new Self(std::forward<Args>(args)...));
            child->parent_ = SFT::shared_from_this();
            childs_.push_back(child);
            return *child;
        }

        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor) const
        {
            acc = ftor(acc, *this);
            for (const auto &child: childs_)
                acc = child->accumulate(acc, ftor);
            return acc;
        }

        //Root is at depth 0
        unsigned int depth() const
        {
            unsigned int depth = 0;
            auto n = SFT::shared_from_this();
            while (!!n)
            {
                ++depth;
                n = n->parent_.lock();
            }
            return depth-1;
        }

    private:
        template <typename Data_>
        friend class Model;

        template <typename ... Args>
        Node(Args ... args): Data(std::forward<Args>(args)...) {}

        static void path_(Path &path, CPtr ptr)
        {
            auto parent = ptr->parent_.lock();
            if (!!parent)
                path_(path, parent);
            path.push_back(ptr);
        }

        std::vector<Ptr> childs_;
        WPtr parent_;
    };

} } 

#endif
