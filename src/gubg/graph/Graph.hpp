#ifndef HEADER_gubg_graph_Graph_hpp_ALREAD_INCLUDED
#define HEADER_gubg_graph_Graph_hpp_ALREAD_INCLUDED

#include <gubg/naft/Document.hpp>

#include <cassert>
#include <cstdint>
#include <map>
#include <vector>

namespace gubg { namespace graph {

    using Vertex = std::size_t;
    using Vertices = std::vector<Vertex>;
    using AdjacencyLists = std::map<Vertex, Vertices>;

    class Graph
    {
    public:
        void clear();

        void init(const AdjacencyLists &);

        bool valid() const { return !vertex__start_ix_.empty(); }

        std::size_t vertex_count() const
        {
            assert(valid());
            return vertex__start_ix_.size() - 1;
        }

        std::size_t indegree(Vertex v) const
        {
            assert(v < vertex__indegree_.size());
            return vertex__indegree_[v];
        }

        template<typename Ftor>
        void each_out(Vertex v, Ftor &&ftor) const
        {
            assert(v + 1 < vertex__start_ix_.size());
            const auto end_ix = vertex__start_ix_[v + 1];
            for (auto ix = vertex__start_ix_[v]; ix < end_ix; ++ix)
                ftor(adjacencies_[ix]);
        }

        void stream(naft::Node &) const;

    private:
        std::vector<Vertex> adjacencies_;
        std::vector<Vertex> vertex__start_ix_;
        std::vector<std::size_t> vertex__indegree_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Graph &g)
    {
        naft::Document doc{os};
        g.stream(doc);
        return os;
    }

}} // namespace gubg::graph

#endif
