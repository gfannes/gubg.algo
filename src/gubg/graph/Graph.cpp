#include <gubg/Range_macro.hpp>
#include <gubg/graph/Graph.hpp>
#include <gubg/hr.hpp>
#include <gubg/mss.hpp>

#include <algorithm>

namespace gubg { namespace graph {

    void Graph::clear()
    {
        *this = Graph{};
    }

    void Graph::init(const AdjacencyLists &als)
    {
        clear();

        std::size_t edge_count = 0;
        std::size_t vertex_count = 0;

        for (const auto &[v, al] : als)
        {
            vertex_count = std::max(vertex_count, v + 1);
            if (!al.empty())
            {
                auto max_el = std::max_element(RANGE(al));
                vertex_count = std::max(vertex_count, *max_el + 1);
                edge_count += al.size();
            }
        }

        adjacencies_.resize(edge_count);
        vertex__start_ix_.resize(vertex_count + 1);
        vertex__indegree_.resize(vertex_count);

        std::size_t start_ix = 0;
        Vertex vv = 0;
        for (const auto &[v, al] : als)
        {
            while (vv <= v)
                vertex__start_ix_[vv++] = start_ix;

            for (auto v : al)
            {
                adjacencies_[start_ix++] = v;
                ++vertex__indegree_[v];
            }
        }
        assert(start_ix == edge_count);

        while (vv <= vertex_count)
            vertex__start_ix_[vv++] = start_ix;
    }

    void Graph::stream(naft::Node &p) const
    {
        auto n = p.node("Graph");
        for (auto v = 0u; v < vertex_count(); ++v)
        {
            auto nn = n.node("Vertex");
            nn.attr("id", v);
            nn.attr("in_degree", vertex__indegree_[v]);
            each_out(v, [&](auto out) { nn.attr("out", out); });
        }
    }

}} // namespace gubg::graph
