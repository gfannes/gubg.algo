#ifndef HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Node.hpp"
#include "gubg/graph/TopologicalOrder.hpp"
#include "gubg/Range.hpp"
#include "gubg/debug.hpp"
#include "gubg/mss.hpp"
#include <map>
#include <set>
#include <utility>

namespace gubg { namespace xtree { 

    //Allows data to be stored in a tree
    //Normal tree-based dependencies are taken for granted (edges from parent to child)
    //Additional dependencies can be added (cross-tree dependencies), but they should not create cycles
    //When aggregating data over the tree, both the tree-based and xtree-based dependencies are taken into account
    //Care is taken to ensure dependencies are incorporated only once
    //The binary data aggregation operation should be commutative `f(a,b) = f(b,a)` and associative `f(f(a,b),c) = f(a,f(b,c))`

    template <typename Data>
    class Model
    {
    private:
        using Self = Model<Data>;

    public:
        using Node = xtree::Node<Data>;
        using Path = typename Node::Path;
        using Node_ptr = typename Node::Ptr;
        using Node_cptr = typename Node::CPtr;

        Model(): root_(new Node) {}

        void clear() {*this = Self();}

        Node &root() {return *root_;}
        Node_ptr &root_ptr() {return root_;}
        Node_cptr root_cptr() const {return root_;}

        //Simple iteration over the tree, without the xlinks
        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor) const
        {
            return root_->accumulate(acc, ftor);
        }
        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor)
        {
            return root_->accumulate(acc, ftor);
        }

        //Iteration over the tree, with xlinks
        //Second argument to ftor() indicates if we are entering (true) or leaving this node (false)
        template <typename Ftor>
        bool traverse(Ftor &&ftor, bool once = true, Node_cptr node = nullptr) const
        {
            MSS_BEGIN(bool);
            if (!node)
                node = root_;
            if (!once)
                return node->traverse(ftor);
            MSS(!!size_);
            std::vector<bool> seen(*size_, false);
            MSS(node->traverse(ftor, &seen));
            MSS_END();
        }
        template <typename Ftor>
        bool traverse(Ftor &&ftor, bool once = true, Node_ptr node = nullptr)
        {
            MSS_BEGIN(bool);
            if (!node)
                node = root_;
            if (!once)
                return node->traverse(ftor);
            MSS(!!size_);
            std::vector<bool> seen(*size_, false);
            MSS(node->traverse(ftor, &seen));
            MSS_END();
        }

        //Iterates over topological order, from leaf to root
        template <typename Ftor>
        bool leaf_to_root(Ftor && ftor)
        {
            MSS_BEGIN(bool);
            for (const auto &node_ptr: leaf_to_root_.order) { MSS(ftor(*node_ptr)); }
            MSS_END();
        }
        template <typename Ftor>
        bool leaf_to_root(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (const auto &node_ptr: leaf_to_root_.order) { MSS(ftor(*node_ptr)); }
            MSS_END();
        }

        //Iterates over topological order, from leaf to root
        template <typename Ftor>
        bool root_to_leaf(Ftor && ftor)
        {
            MSS_BEGIN(bool);
            for (const auto &node_ptr: root_to_leaf_.order) { MSS(ftor(*node_ptr)); }
            MSS_END();
        }
        template <typename Ftor>
        bool root_to_leaf(Ftor && ftor) const
        {
            MSS_BEGIN(bool);
            for (const auto &node_ptr: root_to_leaf_.order) { MSS(ftor(*node_ptr)); }
            MSS_END();
        }
        
        //Aggregation over the tree, from leaf to root, with xlinks
        //ftor(dst, src) should aggregate a child or xlink (src) into the parent (dst)
        template <typename Ftor>
        bool aggregate(Ftor &&ftor)
        {
            MSS_BEGIN(bool);

            MSS(root_->aggregate_tree(ftor));
            
            root_to_leaf([&](auto &node){
                    return node.each_sub([&](auto &sub){return ftor(node, sub);});
                    });

            MSS_END();
        }

        bool process_xlinks()
        {
            return process_xlinks([](const auto &node, const auto &msg){});
        }

        template <typename Problem_cb>
        bool process_xlinks(Problem_cb && problem_cb)
        {
            MSS_BEGIN(bool);

            root_to_leaf_.clear();
            size_ = 0;
            auto process_topo = [&](bool ok, auto &node)
            {
                node.ix_ = (*size_)++;
                node.xsubs_.clear();

                auto each_out_edge = [](const Node_ptr &n, auto &&ftor){
                    n->each_child([&](auto &s){return ftor(s.shared_from_this());});
                    n->each_out([&](auto &s){return ftor(s.shared_from_this());});
                    return true;
                };
                AGG(ok, root_to_leaf_.process(node.shared_from_this(), each_out_edge, true));
                return ok;
            };
            MSS(root_->accumulate(true, process_topo));

            leaf_to_root_ = root_to_leaf_;
            std::reverse(RANGE(leaf_to_root_.order));

            auto lambda = [&](auto &node) {
                std::list<Node_ptr> xsubs;
                node.each_out([&](auto &xout) { xsubs.push_back(xout.shared_from_this()); return true; });
                auto lambda = [&](auto &child)
                {
                    xsubs.insert(xsubs.end(), RANGE(child.xsubs_));
                    return true;
                };
                node.each_child(lambda);
                for (const auto &ptr: xsubs)
                    if (!node.is_far_child(*ptr))
                        node.xsubs_.push_back(ptr);
                return true;
            };
            MSS(leaf_to_root(lambda));

            MSS_END();
        }

    private:
        std::optional<std::size_t> size_;
        Node_ptr root_;
        graph::TopologicalOrder<Node_ptr> root_to_leaf_;
        graph::TopologicalOrder<Node_ptr> leaf_to_root_;
    };

} } 

#endif
