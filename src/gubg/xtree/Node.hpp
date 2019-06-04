#ifndef HEADER_gubg_xtree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Node_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Path.hpp"
#include "gubg/mss.hpp"
#include <vector>
#include <list>
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

        //Adds a child to the tree
        template <typename ... Args>
        Self &emplace_back(Args ... args)
        {
            Ptr child(new Self(std::forward<Args>(args)...));
            child->parent_ = SFT::shared_from_this();
            childs_.push_back(child);
            return *child;
        }

        CPtr parent() const {return parent_.lock();}
        Ptr parent() {return parent_.lock();}

        //Adds a link from this to dst
        Self &add_link(Self &dst)
        {
            dst.xins_.push_back(SFT::weak_from_this());
            xouts_.push_back(dst.weak_from_this());
            return *this;
        }

        template <typename Ftor>
        bool each_child(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (auto &ptr: childs_)
            {
                //We assume no stale childs are present
                assert(!!ptr);
                MSS_Q(ftor(*ptr));
            }
            MSS_END();
        }
        template <typename Ftor>
        bool each_out(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (auto &wptr: xouts_)
            {
                auto ptr = wptr.lock();
                //We assume no stale links are present
                assert(!!ptr);
                MSS_Q(ftor(*ptr));
            }
            MSS_END();
        }
        template <typename Ftor>
        bool each_in(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (auto &wptr: xins_)
            {
                auto ptr = wptr.lock();
                //We assume no stale links are present
                assert(!!ptr);
                MSS_Q(ftor(*ptr));
            }
            MSS_END();
        }
        template <typename Ftor>
        bool each_sub(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (auto &wptr: xsubs_)
            {
                auto ptr = wptr.lock();
                //We assume no stale links are present
                assert(!!ptr);
                MSS_Q(ftor(*ptr));
            }
            MSS_END();
        }

        //Simple iteration over the tree, without the xlinks
        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor) const
        {
            acc = ftor(acc, *this);
            for (const auto &child: childs_)
                acc = child->accumulate(acc, ftor);
            return acc;
        }
        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor)
        {
            acc = ftor(acc, *this);
            for (const auto &child: childs_)
                acc = child->accumulate(acc, ftor);
            return acc;
        }

        //Iteration over the tree, with xlinks
        //Second argument to ftor() indicates if we are entering (true) or leaving this node (false)
        template <typename Ftor>
        bool traverse(Ftor &&ftor, bool once) const
        {
            if (once && visited_)
                return true;

            MSS_BEGIN(bool);
            MSS(ftor(*this, true));
            for (const auto &child: childs_)
            {
                MSS(child->traverse(ftor, once));
            }
            for (const auto &wptr: xouts_)
            {
                auto ptr = wptr.lock();
                MSS(ptr->traverse(ftor, once));
            }
            MSS(ftor(*this, false));
            visited_ = true;
            MSS_END();
        }

        //Aggregation over the tree, from leaf to root, without xlinks
        //ftor(dst, src) should aggregate a child (src) into the parent (dst)
        template <typename Ftor>
        bool aggregate_tree(Ftor &&ftor) const
        {
            MSS_BEGIN(bool);
            for (const auto &child: childs_)
            {
                MSS(child->aggregate_tree(ftor));
                MSS(ftor(*this, *child));
            }
            MSS_END();
        }
        template <typename Ftor>
        bool aggregate_tree(Ftor &&ftor)
        {
            MSS_BEGIN(bool);
            for (const auto &child: childs_)
            {
                MSS(child->aggregate_tree(ftor));
                MSS(ftor(*this, *child));
            }
            MSS_END();
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

        //Keeps track of cross-node links
        //This information is specified by the user via add_link()
        std::list<WPtr> xins_;
        std::list<WPtr> xouts_;

        //Indicates what cross-subtrees are reachable from this node
        //This information is computed and distributed over the tree based on the xins_ and xouts_
        std::list<WPtr> xsubs_;

        mutable bool visited_ = false;
    };

} } 

#endif
