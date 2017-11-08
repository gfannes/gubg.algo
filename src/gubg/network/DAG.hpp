#ifndef HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED

#include "gubg/network/Edge.hpp"
#include "gubg/mss.hpp"
#include <set>
#include <vector>
#include <cassert>

namespace gubg { namespace network { 

    template <typename T>
    class DAG
    {
    public:
        using Self = DAG<T>;
        using Vertices = std::set<T*>;
        using Edge = network::Edge<T>;
        using Edges = std::vector<Edge>;

        void clear(){*this = Self{};}

        bool add_vertex(T *v)
        {
            MSS_BEGIN(bool);
            MSS(!!v);
            MSS(vertices_.insert(v).second);
            MSS_END();
        }

        bool add_edge(T *src, T *dst)
        {
            MSS_BEGIN(bool);
            MSS(!!src && !!dst);
            MSS(out_edges_[src].insert(dst).second);
            MSS_END();
        }

        template <typename Ftor>
        bool each_vertex(Ftor ftor)
        {
            MSS_BEGIN(bool);
            for (auto v: vertices_)
            {
                MSS(ftor(v));
            }
            MSS_END();
        }
        template <typename Ftor>
        bool each_vertex(Ftor ftor) const
        {
            MSS_BEGIN(bool);
            for (auto v: vertices_)
            {
                MSS(ftor(v));
            }
            MSS_END();
        }

        bool remove_unreachables(T *v)
        {
            MSS_BEGIN(bool);
            MSS(vertices_.count(v) > 0);
            Vertices pruned;
            Vertices stage = {v}, new_stage;
            while (!stage.empty())
            {
                assert(new_stage.empty());

                for (auto src: stage)
                    for (auto dst: out_edges_[src])
                    {
                        MSS(pruned.count(dst) == 0);
                        MSS(stage.count(dst) == 0);
                        new_stage.insert(dst);
                    }

#ifdef _MSC_VER
                pruned.insert(stage.begin(), stage.end());
                stage.clear();
#else
                pruned.merge(stage);
                MSS(stage.empty());
#endif

                stage.swap(new_stage);
            }
            vertices_.swap(pruned);
            MSS_END();
        }

    private:
        Vertices vertices_;
        using EdgesPerVertex = std::map<T*, Vertices>;
        EdgesPerVertex out_edges_;
    };

} } 

#endif
