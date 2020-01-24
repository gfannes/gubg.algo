#ifndef HEADER_gubg_gp_tree_Function_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Function_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Node.hpp>
#include <cassert>

namespace gubg { namespace gp { namespace tree { 

    //Function functionality. A Function is a non-leaf Node.
    template <typename Node_, typename Operation>
    class Function: public Node_
    {
    public:
        using Node = Node_;
        using Self = Function<Node, Operation>;
        using Base = typename Node::Base;
        using Ptr = typename Node::Ptr;
        using Childs = typename Node::Childs;
        using Childs_const = typename Node::Childs_const;

        Function(const Operation &operation): operation_(operation)
        {
            childs_.resize(operation_.size());
        }

        Ptr clone(bool deep) override
        {
            Self *raw = new Self{operation_};
            Ptr ptr{raw};
            if (deep)
            {
                const auto nr_childs = childs_.size();
                raw->childs_.resize(nr_childs);
                for (auto i = 0u; i < nr_childs; ++i)
                    if (childs_[i])
                        raw->childs_[i] = childs_[i]->clone(deep);
            }
            return ptr;
        }
        Childs childs() override
        {
            auto ptr = childs_.data();
            return Childs{ptr, ptr+childs_.size()};
        }
        Childs_const childs_const() const override
        {
            auto ptr = childs_.data();
            return Childs_const{ptr, ptr+childs_.size()};
        }
        Base &base() override { return operation_; }

    private:
        std::vector<Ptr> childs_;
        Operation operation_;
    };

    template <typename Node, typename Operation>
    typename Node::Ptr create_function(const Operation &operation)
    {
        typename Node::Ptr ptr{new Function<Node, Operation>{operation}};
        return ptr;
    }

} } } 

#endif
