#ifndef HEADER_gubg_gp_Tree_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_Tree_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <memory>

namespace gubg { namespace gp { 

    template <typename T>
    class Node
    {
    public:
        using Self = Node<T>;
        using Ptr = std::shared_ptr<Self>;
        using Childs = Range<Ptr*>;

        virtual Childs childs() = 0;
        virtual bool compute(T &) const = 0;

    private:
    };

    template <typename T, typename Operation>
    class Function: public Node<T>
    {
    public:
        using Base = Node<T>;
        using Childs = typename Base::Childs;

        Function(Operation &operation): operation_(operation)
        {
            childs_.resize(operation_.size());
        }

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

    private:
        Operation &operation_;
        std::vector<typename Base::Ptr> childs_;
        mutable std::vector<T> tmp_values_;
    };

    template <typename T, typename Operation>
    typename Node<T>::Ptr create_function(Operation &operation)
    {
        typename Node<T>::Ptr ptr{new Function<T, Operation>{operation}};
        return ptr;
    }

    template <typename T, typename Value>
    class Terminal: public Node<T>
    {
    public:
        using Base = Node<T>;
        using Childs = typename Base::Childs;

        Terminal(Value &value): value_(value) {}

        Childs childs() override { return Childs{nullptr, nullptr}; }
        bool compute(T &v) const override { return value_.compute(v); }

    private:
        Value &value_;
    };

    template <typename T, typename Value>
    typename Node<T>::Ptr create_terminal(Value &value)
    {
        typename Node<T>::Ptr ptr{new Terminal<T, Value>{value}};
        return ptr;
    }

} } 

#endif
