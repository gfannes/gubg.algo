#ifndef HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace gubg { namespace gp { namespace tree { 

    //Indicator used during depth-first search to communicate the node type and iteration status
    enum class DFS {Open, Close, Terminal};

    //Node<T, Base> is an interface that allows evaluation and depth-first search iteration.
    //Terminal<Node, Value> and Function<Node, Operation> are concrete instances that implement the Node<> interface,
    //and handles storage of childs when necessary. The Value and Operation classes represent the details of the Terminal
    //or Function. Value and Operation are stored as members in resp. Terminal and Function.
    //During depth-first search, a const Base * is received, as such is it important that both Value and Operation can
    //be cast to const Base *.
    template <typename T_, typename Base_ = void>
    class Node
    {
    public:
        using T = T_;
        using Base = Base_;
        using Self = Node<T, Base>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;
        using Functor = std::function<void(const Base *, DFS)>;

        virtual ~Node(){}
        virtual Ptr clone() const = 0;
        virtual Childs childs() = 0;
        virtual bool compute(T &) = 0;
        virtual void dfs(Functor &ftor) const = 0;

        std::size_t size() const
        {
            std::size_t nr = 0;
            auto visitor = [&](const auto &data, DFS dfs)
            {
                switch (dfs)
                {
                    case DFS::Open:
                    case DFS::Terminal:
                        break;
                }
            }
            dfs(visitor);
            return nr;
        }

    private:
    };

    template <typename Node_, typename Operation>
    class Function: public Node_
    {
    public:
        using Node = Node_;
        using Self = Function<Node, Operation>;
        using T = typename Node::T;
        using Childs = typename Node::Childs;
        using Ptr = typename Node::Ptr;
        using Functor = typename Node::Functor;

        Function(const Operation &operation): operation_(operation)
        {
            childs_.resize(operation_.size());
        }

        Ptr clone() const { return Ptr{new Self{operation_}}; }
        Childs childs() override
        {
            auto ptr = childs_.data();
            return Childs{ptr, ptr+childs_.size()};
        }
        bool compute(T &v) override
        {
            return operation_.compute(v, childs_);
        }
        void dfs(Functor &ftor) const override
        {
            ftor(&operation_, DFS::Open);
            for (const auto &child: childs_)
                child->dfs(ftor);
            ftor(&operation_, DFS::Close);
        }

    private:
        Operation operation_;
        std::vector<typename Node::Ptr> childs_;
    };

    template <typename Node, typename Operation>
    typename Node::Ptr create_function(const Operation &operation)
    {
        typename Node::Ptr ptr{new Function<Node, Operation>{operation}};
        return ptr;
    }

    template <typename Node_, typename Value>
    class Terminal: public Node_
    {
    public:
        using Node = Node_;
        using Self = Terminal<Node, Value>;
        using T = typename Node::T;
        using Childs = typename Node::Childs;
        using Ptr = typename Node::Ptr;
        using Functor = typename Node::Functor;

        Terminal(const Value &value): value_(value) {}

        Ptr clone() const { return Ptr{new Self{value_}}; }
        Childs childs() override { return Childs{nullptr, nullptr}; }
        bool compute(T &v) override { return value_.compute(v); }
        void dfs(Functor &ftor) const override {ftor(&value_, DFS::Terminal);}

    private:
        Value value_;
    };

    template <typename Node, typename Value>
    typename Node::Ptr create_terminal(const Value &value)
    {
        typename Node::Ptr ptr{new Terminal<Node, Value>{value}};
        return ptr;
    }

} } } 

#endif
