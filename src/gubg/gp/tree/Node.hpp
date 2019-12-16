#ifndef HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace gubg { namespace gp { namespace tree { 

#define gp_new 1
#if gp_new
    using Path = std::vector<unsigned int>;

    template <typename Visitor_>
    class Node
    {
    public:
        using Visitor = Visitor_;
        using Self = Node<Visitor>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;

        virtual ~Node() {}
        virtual Ptr clone(bool deep) = 0;
        virtual void visit(Visitor &) = 0;
        virtual Childs childs() = 0;
    private:
    };

    template <typename Visitor, typename Ftor>
    bool dfs(typename Node<Visitor>::Ptr &root, Ftor &&ftor, Path &path)
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

    template <typename Node_, typename Operation>
    class Function: public Node_
    {
    public:
        using Node = Node_;
        using Self = Function<Node, Operation>;
        using Visitor = typename Node::Visitor;
        using Ptr = typename Node::Ptr;
        using Childs = typename Node::Childs;

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
        void visit(Visitor &v) override { v(operation_); }

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

    template <typename Node_, typename Value>
    class Terminal: public Node_
    {
    public:
        using Node = Node_;
        using Self = Terminal<Node, Value>;
        using Visitor = typename Node::Visitor;
        using Ptr = typename Node::Ptr;
        using Childs = typename Node::Childs;

        Terminal(const Value &value): value_(value) {}

        Ptr clone(bool deep) override { return Ptr{new Self{value_}}; }
        Childs childs() override { return Childs{nullptr, nullptr}; }
        void visit(Visitor &v) override { v(value_); }

    private:
        Value value_;
    };

    template <typename Node_, typename Value>
    typename Node_::Ptr create_terminal(const Value &value)
    {
        typename Node_::Ptr ptr{new Terminal<Node_, Value>{value}};
        return ptr;
    }

#else
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
            Functor visitor = [&](const Base *data, DFS dfs)
            {
                switch (dfs)
                {
                    case DFS::Open:
                    case DFS::Terminal:
                        ++nr;
                        break;
                }
            };
            dfs(visitor);
            return nr;
        }
        const Base *get_data(std::size_t ix) const
        {
            const Base *d = nullptr;
            std::size_t nr = 0;
            Functor visitor = [&](const Base *data, DFS dfs)
            {
                if (d)
                    return;
                if (nr == ix)
                    d = data;
                switch (dfs)
                {
                    case DFS::Open:
                    case DFS::Terminal:
                        ++nr;
                        break;
                }
            };
            dfs(visitor);
            return d;
        }
        Self *get_node(std::size_t ix)
        {
            std::size_t nr = 0;
            return get_node_(ix, nr);
        }

    private:
        Self *get_node_(std::size_t ix, std::size_t &nr)
        {
            if (nr == ix)
                return this;
            ++nr;
            for (const auto &child: childs())
            {
                auto ptr = child->get_node_(ix, nr);
                if (ptr)
                    return ptr;
            }
            return nullptr;
        }
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
#endif

} } } 

#endif
