#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <vector>
#include <memory>

namespace gubg { namespace tree { 

    using Path = std::vector<size_t>;

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
    };

    class NoData { };

    template <typename Node, typename Data_ = NoData>
    class LeafNode: public Node, public Data_
    {
    public:
        using Self = LeafNode<Node, Data_>;
        using Data = Data_;
        using Ptr = typename Node::Ptr;
        using Visitor = typename Node::Visitor;
        using Childs = typename Node::Childs;

        LeafNode() {}
        LeafNode(const Data &data): Data_(data) {}

        Data &data() {return static_cast<Data&>(*this);}

        Ptr clone(bool deep) override { return Ptr{new Self{data()}}; }
        void visit(Visitor &v) override {v(data());}
        Childs childs() override { return Childs{nullptr, nullptr}; }
    };
    template <typename Node, typename Data = NoData>
    typename Node::Ptr create_leaf(const Data &data = NoData{})
    {
        using Type = LeafNode<Node, Data>;
        return typename Node::Ptr{new Type{data}};
    }

    template <typename Node, typename Data_ = NoData>
    class BranchNode: public Node, public Data_
    {
    public:
        using Self = BranchNode<Node, Data_>;
        using Data = Data_;
        using Ptr = typename Node::Ptr;
        using Visitor = typename Node::Visitor;
        using Childs = typename Node::Childs;

        BranchNode() {}
        BranchNode(const Data &data): Data_(data) {}
        BranchNode(const Data &data, size_t nr_childs): Data_(data), childs_(nr_childs) {}

        Data &data() {return static_cast<Data&>(*this);}

        Ptr clone(bool deep) override
        {
            auto raw = new Self{data()};
            Ptr ptr{raw};
            if (deep)
            {
                const auto size = childs_.size();
                raw->childs_.resize(size);
                for (auto ix = 0; ix < size; ++ix)
                    raw->childs_[ix] = childs_[ix]->clone(deep);
            }
            return ptr;
        }
        void visit(Visitor &v) override {v(data());}
        Childs childs() override
        {
            auto ptr = childs_.data();
            return Childs{ptr, ptr+childs_.size()};
        }

    private:
        std::vector<Ptr> childs_;
    };
    template <typename Node, typename Data = NoData>
    typename Node::Ptr create_branch(size_t nr_childs, const Data &data = NoData{})
    {
        using Type = BranchNode<Node, Data>;
        return typename Node::Ptr{new Type{data, nr_childs}};
    }

} } 

#endif
