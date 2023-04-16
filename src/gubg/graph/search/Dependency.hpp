#ifndef HEADER_gubg_graph_search_Dependency_hpp_ALREAD_INCLUDED
#define HEADER_gubg_graph_search_Dependency_hpp_ALREAD_INCLUDED

#include <gubg/debug.hpp>
#include <gubg/graph/Graph.hpp>
#include <gubg/hr.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace graph { namespace search {

    class Dependency
    {
    public:
        bool init(const Graph *g);
        bool valid() const { return !!g_; }

        void reset();

        bool next(Vertex &);

        template<typename Ftor>
        bool each(Ftor &&ftor)
        {
            MSS_BEGIN(bool, "");

            MSS(valid());

            reset();
            std::size_t count = 0;
            for (Vertex v; next(v); ftor(v), ++count) {}

            MSS(count == g_->vertex_count(), L("This is not a DAG"));

            MSS_END();
        }

        bool topo_order(Vertices &);

    private:
        bool is_unlocked_(Vertex) const;

        const graph::Graph *g_ = nullptr;
        std::vector<std::size_t> vertex__incount_;
        Vertices unlocked_;
        Vertices unlocked_tmp_;
    };

}}} // namespace gubg::graph::search

#endif
