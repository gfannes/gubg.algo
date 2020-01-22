#ifndef HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace gubg { namespace gp { namespace tree { 

    //Path into a tree
    using Path = std::vector<unsigned int>;

    //Abstract base class for Terminals and Functions
    //Base_ is the base type for Terminal values and Function operations
    //Via the Node.base() method, you can get to this base
    //Node, Terminal and Function are responsible for the tree structure
    //If you need access to Node from Base, you have to pass it explicitly
    template <typename Base_>
    class Node
    {
    public:
        using Base = Base_;
        using Self = Node<Base>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;
        using Childs_const = Range<const Ptr*>;

        std::size_t size() const
        {
            std::size_t nr = 0;
            size_recursive_(nr);
            return nr;
        }

        virtual ~Node() {}
        virtual Ptr clone(bool deep) = 0;
        virtual Base &base() = 0;
        virtual Childs childs() = 0;
        virtual Childs_const childs_const() const = 0;

    private:
        void size_recursive_(std::size_t &nr) const
        {
            ++nr;
            for (auto child_ptr: childs_const())
                child_ptr->size_recursive_(nr);
        }
    };

    template <typename Base, typename Ftor>
    bool dfs(typename Node<Base>::Ptr &root, Ftor &&ftor, Path &path)
    {
        MSS_BEGIN(bool);
        MSS(!!root);
        MSS(ftor(root, path, true));
        path.push_back(0);
        for (auto &child: root->childs())
        {
            MSS(dfs(child, ftor, path));
            ++path.back();
        }
        path.pop_back();
        MSS(ftor(root, path, false));
        MSS_END();
    }


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
