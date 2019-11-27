#ifndef HEADER_gubg_gp_tree_Grow_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Grow_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Node.hpp>
#include <gubg/mss.hpp>
#include <optional>
#include <random>

namespace gubg { namespace gp { namespace tree { 

    template <typename T>
    class Grow
    {
    public:
        using Ptr = typename Node<T>::Ptr;

        bool set_probs(double terminal, double function)
        {
            MSS_BEGIN(bool);
            prob_terminal_.reset();
            MSS(terminal >= 0 && function >= 0 && terminal+function > 0);
            prob_terminal_ = terminal/(terminal+function);
            MSS_END();
        }
        bool set_max_depth(unsigned int max_depth)
        {
            MSS_BEGIN(bool);
            max_depth_ = max_depth;
            MSS_END();
        }

        template <typename Terminals, typename Functions>
        bool operator()(Ptr &root, const Terminals &terminals, const Functions &functions)
        {
            MSS_BEGIN(bool);
            MSS(!!prob_terminal_);
            MSS(!!max_depth_);
            MSS(grow_(root, terminals, functions, 0));
            MSS_END();
        }

    private:
        template <typename Terminals, typename Functions>
        bool grow_(Ptr &node, const Terminals &terminals, const Functions &functions, unsigned int depth)
        {
            MSS_BEGIN(bool, "");
            if (depth < *max_depth_)
            {
                //We can still choose between terminals or functions
                if (choose_(*prob_terminal_))
                {
                    MSS(select_(node, terminals));
                }
                else
                {
                    MSS(select_(node, functions));
                    for (auto &child: node->childs())
                    {
                        MSS(grow_(child, terminals, functions, depth+1));
                    }
                }
            }
            else
            {
                //Max depth is reached: only terminals can be selected
                MSS(select_(node, terminals));
            }
            MSS_END();
        }

        bool choose_(double prob_true)
        {
            std::uniform_real_distribution<> uniform;
            return uniform(rng_) < prob_true;
        }

        template <typename Nodes>
        bool select_(Ptr &node, const Nodes &nodes)
        {
            MSS_BEGIN(bool, "");
            const auto size = nodes.size();
            MSS(size > 0);
            std::uniform_int_distribution<> uniform{0, size-1};
            const auto ix = uniform(rng_);
            auto &selected = nodes[ix];
            MSS(!!selected);
            node = selected->clone();
            MSS_END();
        }

        std::optional<double> prob_terminal_;
        std::optional<unsigned int> max_depth_;
        std::mt19937 rng_;
    };

} } } 

#endif
