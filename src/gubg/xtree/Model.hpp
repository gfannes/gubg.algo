#ifndef HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Model_hpp_ALREADY_INCLUDED

#include "gubg/xtree/Node.hpp"

namespace gubg { namespace xtree { 

    template <typename Data>
    class Model
    {
    private:
        using Self = Model<Data>;

    public:
        using Node = xtree::Node<Data>;
        using Path = typename Node::Path;

        Model(): root_(new Node) {}

        void clear() {*this = Self();}

        Node &root() {return *root_;}

        template <typename Acc, typename Ftor>
        Acc accumulate(Acc acc, Ftor &&ftor) const
        {
            return root_->accumulate(acc, ftor);
        }

    private:
        typename Node::Ptr root_;
    };

} } 

#endif
