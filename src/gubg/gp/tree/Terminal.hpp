#ifndef HEADER_gubg_gp_tree_Terminal_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Terminal_hpp_ALREADY_INCLUDED

#include <gubg/gp/tree/Node.hpp>

namespace gubg { namespace gp { namespace tree { 

    //Terminal functionality. A Terminal is a leaf Node.
    template <typename Node_, typename Value>
    class Terminal: public Node_
    {
    public:
        using Node = Node_;
        using Self = Terminal<Node, Value>;
        using Base = typename Node::Base;
        using Ptr = typename Node::Ptr;
        using Childs = typename Node::Childs;
        using Childs_const = typename Node::Childs_const;

        Terminal(const Value &value): value_(value) {}

        Ptr clone(bool deep) override { return Ptr{new Self{value_}}; }
        Childs childs() override { return Childs{nullptr, nullptr}; }
        Childs_const childs_const() const override { return Childs_const{nullptr, nullptr}; }
        Base &base() override { return value_; }

    private:
        Value value_;
    };

    template <typename Node_, typename Value>
    typename Node_::Ptr create_terminal(const Value &value)
    {
        typename Node_::Ptr ptr{new Terminal<Node_, Value>{value}};
        return ptr;
    }

} } } 

#endif
