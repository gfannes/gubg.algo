#ifndef HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Node.hpp"
#include "gubg/Range.hpp"
#include "gubg/debug.hpp"
#include <map>
#include <set>
#include <utility>

namespace gubg { namespace xtree { 

    template <typename Data>
    class Model
    {
    private:
        using Self = Model<Data>;

    public:
        using Node = xtree::Node<Data>;
        using Path = typename Node::Path;
        using Node_ptr = typename Node::Ptr;

        Model(): root_(new Node) {}

        void clear() {*this = Self();}

        Node &root() {return *root_;}

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
