#ifndef HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_Node_hpp_ALREADY_INCLUDED

#include <gubg/Range.hpp>
#include <gubg/mss.hpp>
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
        virtual size_t size() const = 0;
        virtual bool resize(size_t) = 0;
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
        size_t size() const override {return 0;}
        bool resize(size_t size) override {return size == 0;}
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
        size_t size() const override {return childs_.size();}
        bool resize(size_t size) override {childs_.resize(size); return true;}

    private:
        std::vector<Ptr> childs_;
    };
    template <typename Node, typename Data = NoData>
    typename Node::Ptr create_branch(const Data &data = NoData{}, size_t nr_childs = 0)
    {
        using Type = BranchNode<Node, Data>;
        return typename Node::Ptr{new Type{data, nr_childs}};
    }

    //ftor is called as ftor(Ptr &node, Path &path, bool enter)
    template <typename NodePtr, typename Ftor>
    bool dfs(NodePtr &root, Ftor &&ftor, Path &path)
    {
        MSS_BEGIN(bool);
        MSS(ftor(root, path, true));
        if (root)
        {
            path.push_back(0);
            for (auto &child: root->childs())
            {
                MSS(dfs(child, ftor, path));
                ++path.back();
            }
            path.pop_back();
        }
        MSS(ftor(root, path, false));
        MSS_END();
    }

    namespace details { 
        template <typename NodePtr>
        bool search(NodePtr *&dst, NodePtr &node, const Path &path, size_t ix)
        {
            MSS_BEGIN(bool);
            if (path.size() == ix)
            {
                //We found the node
                dst = &node;
            }
            else
            {
                MSS(!!node);
                const auto child_ix = path[ix];
                auto childs = node->childs();
                MSS(child_ix < childs.size());
                MSS(search(dst, childs[child_ix], path, ix+1));
            }
            MSS_END();
        }
    } 
    template <typename NodePtr>
    bool search(NodePtr *&dst, NodePtr &root, const Path &path)
    {
        return details::search(dst, root, path, 0);
    }

} } 

#endif
