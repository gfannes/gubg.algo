#ifndef HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace gubg { namespace gp { namespace tree { 

    enum class DFS {Open, Close, Terminal};

    template <typename T>
    class Node
    {
    public:
        using Self = Node<T>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;
        using Functor = std::function<void(const Self *, DFS)>;

        virtual Ptr clone() const = 0;
        virtual Childs childs() = 0;
        virtual bool compute(T &) const = 0;
        virtual void dfs(Functor &ftor) const = 0;

    private:
    };

    template <typename T, typename Operation>
    class Function: public Node<T>
    {
    public:
        using Self = Function<T, Operation>;
        using Base = Node<T>;
        using Childs = typename Base::Childs;
        using Ptr = typename Base::Ptr;
        using Functor = typename Base::Functor;

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
        bool compute(T &v) const override
        {
            MSS_BEGIN(bool);
            tmp_values_.resize(childs_.size());
            auto value_it = tmp_values_.begin();
            for (const auto &child: childs_)
            {
                MSS(!!child);
                MSS(child->compute(*value_it));
                ++value_it;
            }
            auto ptr = tmp_values_.data();
            MSS(operation_.compute(v, ptr, ptr+tmp_values_.size()));
            MSS_END();
        }
        void dfs(Functor &ftor) const override
        {
            ftor(this, DFS::Open);
            for (const auto &child: childs_)
                child->dfs(ftor);
            ftor(this, DFS::Close);
        }

    private:
        Operation operation_;
        std::vector<typename Base::Ptr> childs_;
        mutable std::vector<T> tmp_values_;
    };

    template <typename T, typename Operation>
    typename Node<T>::Ptr create_function(const Operation &operation)
    {
        typename Node<T>::Ptr ptr{new Function<T, Operation>{operation}};
        return ptr;
    }

    template <typename T, typename Value>
    class Terminal: public Node<T>
    {
    public:
        using Self = Terminal<T, Value>;
        using Base = Node<T>;
        using Childs = typename Base::Childs;
        using Ptr = typename Base::Ptr;
        using Functor = typename Base::Functor;

        Terminal(const Value &value): value_(value) {}

        Ptr clone() const { return Ptr{new Self{value_}}; }
        Childs childs() override { return Childs{nullptr, nullptr}; }
        bool compute(T &v) const override { return value_.compute(v); }
        void dfs(Functor &ftor) const override {ftor(this, DFS::Terminal);}

    private:
        Value value_;
    };

    template <typename T, typename Value>
    typename Node<T>::Ptr create_terminal(const Value &value)
    {
        typename Node<T>::Ptr ptr{new Terminal<T, Value>{value}};
        return ptr;
    }

} } } 

#endif
