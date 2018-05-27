#ifndef HEADER_gubg_graph_TopologicalSort_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_TopologicalSort_hpp_ALREADY_INCLUDED

#include "gubg/graph/Traits.hpp"
#include "gubg/Range.hpp"
#include "gubg/mss.hpp"
#include <unordered_map>
#include <stack>
#include <algorithm>

namespace gubg { namespace graph {

    namespace detail {

        template <typename G, typename M>
        void fill_indegree_map(const G & g, M & m, bidirectional)
        {
            for(auto v : vertices(g))
                m[v] = in_degree(v, g);
        }
        
        template <typename G, typename M>
        void fill_indegree_map(const G & g, M & m, directed)
        {
            for (auto e : edges(g))
            {
                auto it = m.insert(std::make_pair(target(e, g), 0)).first;
                ++(it->second);
            }
        }
    }

    template <typename Graph, typename OutIt>
    bool construct_topological_order(const Graph & g, OutIt out_it)
    {
        MSS_BEGIN(bool);

        static_assert(Traits<Graph>::has_direction, "The supplied graph is undirected");
        using vertex_descriptor = typename Traits<Graph>::vertex_descriptor;

	unsigned int count = 0;
        
        // fill a in degree map
        std::unordered_map<vertex_descriptor, unsigned int> in_degree_map;
        detail::fill_indegree_map(g, in_degree_map, typename Traits<Graph>::direction_tag());

        // find all vertices with indegree zero
        std::stack<vertex_descriptor> todo;
        for(const auto & p : in_degree_map)
            if (p.second == 0)
                todo.push(p.first);

        // main processing
        while(!todo.empty())
        {
            vertex_descriptor vd = todo.top();
            todo.pop();

            // get the degree we found 
            auto it = in_degree_map.find(vd);
            MSS(it != in_degree_map.end());
            MSS(it->second == 0);

            *out_it++ = vd;
	    ++count;
            for(auto v : adjacent_vertices(vd, g))
	    {
                auto it2 = in_degree_map.find(v);
                MSS(it2 != in_degree_map.end());
                auto & cnt = it2->second;

                MSS(cnt > 0);
                if(--cnt == 0)
                    todo.push(v);
            }
        }

        // it is a correct topological order if the indegree map contains only zero's after processing
        MSS(std::all_of(RANGE(in_degree_map), [](auto & p) { return p.second == 0; }));
	MSS(count == num_vertices(g));

        MSS_END();
    }   


} }

#endif

