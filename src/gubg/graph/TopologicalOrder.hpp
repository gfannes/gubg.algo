#ifndef HEADER_gubg_graph_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <list>
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

    template <typename Vertex>
    class TopologicalOrder
    {
    private:
        using Self = TopologicalOrder<Vertex>;

    public:
        std::list<Vertex> order;

        void clear() {*this = Self{};}

        template <typename EachOutEdge>
        bool process(const Vertex &n, EachOutEdge &&each_out_edge)
        {
            MSS_BEGIN(bool);
            auto &mark = marks_[n];
            if (mark == Mark::Unvisited)
                MSS(visit_(n, mark, each_out_edge));
            MSS_END();
        }

    private:
        template <typename EachOutEdge>
        bool visit_(const Vertex &n, Mark &mark, EachOutEdge &&each_out_edge)
        {
            MSS_BEGIN(bool);
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
                        return visit_(m, mark, each_out_edge);
                    };
                    MSS(each_out_edge(n, recurse));
                    mark = Mark::Visited;
                    order.push_front(n);
                    break;
            }
            MSS_END();
        }

        std::map<Vertex, Mark> marks_;
    };

} } 

#endif
