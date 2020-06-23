#ifndef HEADER_gubg_graph_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <list>
#include <vector>
#include <map>
#include <ostream>

namespace gubg { namespace graph { 

    enum class Mark {Unvisited, Visiting, Visited};
    inline std::ostream &operator<<(std::ostream &os, Mark mark)
    {
        switch (mark)
        {
            case Mark::Unvisited: os << "Unvisited"; break;
            case Mark::Visiting: os << "Visiting"; break;
            case Mark::Visited: os << "Visited"; break;
        }
        return os;
    }

    //Vertex should be copyable
    template <typename Vertex>
    class TopologicalOrder
    {
    private:
        using Self = TopologicalOrder<Vertex>;

    public:
        std::list<Vertex> order;
        std::vector<Vertex> cycle;

        void clear() {*this = Self{};}

        //If root_to_leaf == true, the resulting order will be from root to leaf
        template <typename EachOutEdge>
        bool process(const Vertex &n, EachOutEdge &&each_out_edge, bool root_to_leaf = true)
        {
            MSS_BEGIN(bool);
            cycle.resize(0);
            auto &mark = marks_[n];
            if (mark == Mark::Unvisited)
                MSS(visit_(n, mark, each_out_edge, root_to_leaf));
            cycle.resize(0);
            MSS_END();
        }

    private:
        template <typename EachOutEdge>
        bool visit_(const Vertex &n, Mark &mark, EachOutEdge &&each_out_edge, bool root_to_leaf)
        {
            MSS_BEGIN(bool);
            cycle.push_back(n);
            switch (mark)
            {
                case Mark::Visited:
                    break;
                case Mark::Visiting:
                    MSS(false);//Not a DAG
                    break;
                default:
                    mark = Mark::Visiting;
                    auto recurse = [&](const Vertex &m){
                        auto &mark = marks_[m];
                        return visit_(m, mark, each_out_edge, root_to_leaf);
                    };
                    MSS(each_out_edge(n, recurse));
                    mark = Mark::Visited;
                    if (root_to_leaf)
                        order.push_front(n);
                    else
                        order.push_back(n);
                    break;
            }
            cycle.pop_back();
            MSS_END();
        }

        std::map<Vertex, Mark> marks_;
    };

} } 

#endif
