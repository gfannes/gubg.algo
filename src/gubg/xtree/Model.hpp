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
        Node_cptr root_ptr() const {return root_;}

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
            if (once)
                root_->accumulate(true, [](bool ok, auto &node){node.visited_ = false; return ok;});
            if (!node)
                node = root_;
            return node->traverse(ftor, once);
        }
        template <typename Ftor>
        bool traverse(Ftor &&ftor, bool once = true, Node_ptr node = nullptr)
        {
            if (once)
                root_->accumulate(true, [](bool ok, auto &node){node.visited_ = false; return ok;});
            if (!node)
                node = root_;
            return node->traverse(ftor, once);
        }

        //Aggregation over the tree, from leaf to root, with xlinks
        //ftor(dst, src) should aggregate a child or xlink (src) into the parent (dst)
        template <typename Ftor>
        bool aggregate(Ftor &&ftor)
        {
            MSS_BEGIN(bool);

            MSS(root_->aggregate_tree(ftor));

            //We process the subs according to topological order
            //to make sure xlinks are not taken into account twice in a situation like:
            //[a]{
            //  [b](dep:a/d){
            //    [c]
            //  }
            //  [d](dep:a/b/c)
            //}
            //Here, potentially, a/b/c is aggregated twice into a/b

            //Compute the topological order:
            //* Process each node via accumulate()
            //* each_out_edge() only iterates over the subs
            graph::TopologicalOrder<Node_ptr> topo;
            auto process_topo = [&](bool ok, auto &node)
            {
                auto each_out_edge = [](const Node_ptr &n, auto &&ftor){
                    return n->each_sub([&](auto &s){return ftor(s.shared_from_this());});
                };
                AGG(ok, topo.process(node.shared_from_this(), each_out_edge));
                return ok;
            };
            MSS(root_->accumulate(true, process_topo));

            //Aggregate the xlinks according to the topological order
            for (const auto &node_ptr: topo.order)
            {
                auto &node = *node_ptr;
                auto lambda = [&](auto &to){
                    return ftor(node, to);
                };
                MSS(node.each_sub(lambda));
            }

            MSS_END();
        }

        bool process_xlinks()
        {
            return process_xlinks([](const auto &node, const auto &msg){});
        }

        template <typename Problem_cb>
        bool process_xlinks(Problem_cb && problem_cb)
        {
            MSS_BEGIN(bool, "");

            using Links = std::map<Node_ptr, std::set<Node_ptr>>;

            Links todo;
            auto lambda = [&](bool ok, auto &node){
                node.xsubs_.clear();
                return ok && node.each_out([&](auto &to){todo[node.shared_from_this()].insert(to.shared_from_this()); return true;});
            };
            MSS(accumulate(true, lambda));

            Links done;
            while (!todo.empty())
            {
                L(C(todo.size()));
                Links current;
                current.swap(todo);
                for (const auto &p: current)
                {
                    auto &from = p.first;
                    for (const auto &to: p.second)
                    {
                        //Distribute this cross-dependency down to the root
                        for (auto node = from; !!node; node = node->parent_.lock())
                        {
                            MSS(node != to, problem_cb(*node, *from, "cycle detected"););
                            done[node].insert(to);

                            //All nodes that depend on node need this new dependency as well,
                            //we will add that to `todo` and process them in the next iteration
                            auto stage_new_dependency = [&](auto &f)
                            {
                                MSS_BEGIN(bool);
                                auto f_ptr = f.shared_from_this();
                                auto &t_ptr = to;
                                if (done[f_ptr].count(t_ptr) == 0)
                                    todo[f_ptr].insert(t_ptr);
                                MSS_END();
                            };
                            MSS(node->each_in(stage_new_dependency));
                        }
                    }
                }
            }

            Path path;
            for (const auto &p: done)
            {
                auto &node_ptr = p.first;
                auto &node = *node_ptr;
                for (const auto &to: p.second)
                {
                    to->path(path);
                    if (std::find_if(RANGE(path), [&](auto &n){return n == node_ptr;}) == path.end())
                        node.xsubs_.push_back(to);
                }
            }
            MSS_END();
        }

    private:
        Node_ptr root_;
    };

} } 

#endif
