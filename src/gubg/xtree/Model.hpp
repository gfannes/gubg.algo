#ifndef HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Node.hpp"
#include "gubg/Range.hpp"
#include "gubg/debug.hpp"
#include <map>
#include <set>
#include <utility>

namespace gubg { namespace xtree { 

    //Allows data to be stored in a tree
    //Normal tree-based dependencies are taken for granted (edges from parent to child)
    //Additional dependencies can be add (cross-tree dependencies), but they should not create cycles
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
        Node_cptr &root_ptr() const {return root_;}

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

        void process_xlinks()
        {
            S("");

            using Links = std::map<Node_ptr, std::set<Node_ptr>>;

            Links todo;
            auto lambda = [&](bool, auto &node){
                node.each_out([&](auto &to){todo[node.shared_from_this()].insert(to.shared_from_this());});
                return true;
            };
            accumulate(true, lambda);

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
                        L(C(from->name)C(to->name));
                        //Distribute this cross-dependency down to the root
                        for (auto node = from; !!node; node = node->parent_.lock())
                        {
                            //All nodes that depend on node need this new dependency as well,
                            //we will add that to `todo` and process them in the next iteration
                            auto stage_new_dependency = [&](auto &f)
                            {
                                auto f_ptr = f.shared_from_this();
                                auto &t_ptr = to;
                                if (done[f_ptr].count(t_ptr) == 0)
                                    todo[f_ptr].insert(t_ptr);
                            };
                            node->each_in(stage_new_dependency);

                            done[node].insert(to);
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
                    //TODO: this should be made more intelligent: instead of simply excluding the x-deps
                    //that are already covered via the tree-based dependencies, there could be a partial overlap
                    //between node and to, and between different to's for the same node
                    if (std::find_if(RANGE(path), [&](auto &n){return n == node_ptr;}) == path.end())
                        node.xsubs_.push_back(to);
                }
            }
        }

    private:
        Node_ptr root_;
    };

} } 

#endif
