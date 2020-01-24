#ifndef HEADER_gubg_gp_tree_Grow_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Grow_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Node.hpp>
#include <gubg/mss.hpp>
#include <optional>
#include <random>

namespace gubg { namespace gp { namespace tree { 

    //Grows a tree by selecting terminals and functions at random.
    //Once the max depth is reached, only terminals are selected.

    template <typename Node>
    class Grow
    {
    public:
        using Ptr = typename Node::Ptr;

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

        template <typename TerminalFactory, typename FunctionFactory>
        bool operator()(Ptr &root, TerminalFactory &&terminal_factory, FunctionFactory &&function_factory)
        {
            MSS_BEGIN(bool);
            MSS(!!prob_terminal_);
            MSS(!!max_depth_);
            MSS(grow_(root, terminal_factory, function_factory, 0));
            MSS_END();
        }

    private:
        template <typename TerminalFactory, typename FunctionFactory>
        bool grow_(Ptr &node, TerminalFactory &&terminal_factory, FunctionFactory &&function_factory, unsigned int depth)
        {
            MSS_BEGIN(bool);
            if (depth < *max_depth_)
            {
                //We can still choose between terminals or functions
                if (choose_(*prob_terminal_))
                {
                    MSS(terminal_factory(node));
                    MSS(!!node);
                    L("Terminal " << C(node.get()));
                }
                else
                {
                    MSS(function_factory(node));
                    MSS(!!node);
                    L("Function " << C(node.get()));
                    for (auto &child: node->childs())
                    {
                        MSS(grow_(child, terminal_factory, function_factory, depth+1));
                    }
                }
            }
            else
            {
                //Max depth is reached: only terminals can be selected
                MSS(terminal_factory(node));
                MSS(!!node);
                L("Terminal " << C(node.get()));
            }
            MSS_END();
        }

        bool choose_(double prob_true)
        {
            std::uniform_real_distribution<> uniform;
            return uniform(rng_) < prob_true;
        }

        std::optional<double> prob_terminal_;
        std::optional<unsigned int> max_depth_;
        std::mt19937 rng_;
    };

} } } 

#endif
