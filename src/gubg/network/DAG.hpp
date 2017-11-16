#ifndef HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <set>
#include <map>
#include <list>
#include <ostream>
#include <cassert>

namespace gubg { namespace network { 

    enum class Direction { Forward, Backward, };

    template <typename Vertex>
    class DAG
    {
    public:
        using Self = DAG<Vertex>;
        using Sequence = std::list<Vertex*>;
        using Set = std::set<Vertex*>;
        struct Info
        {
            int order = 0;
            typename Sequence::iterator it;
            Set dsts;
            Info(){}
            Info(int order): order(order){}
        };
        using InfoPerVertex = std::map<const Vertex*, Info>;
        using Vertices = std::set<const Vertex*>;

        void clear(){*this = Self{};}

        size_t size() const
        {
            assert(invariants_());
            return sequence_.size();
        }

        bool add_vertex(Vertex *v, bool fail_when_present = true)
        {
            MSS_BEGIN(bool, "");
            L(C(v));
            assert(invariants_());
            MSS(!!v);
            int order = -(int)size();
            auto it = info_.insert(std::make_pair(v, Info{order}));
            if (!it.second)
            {
                //Node is already present
                MSS(!fail_when_present);
            }
            else
            {
                //Node should be added
                sequence_.push_front(v);
                it.first->second.it = sequence_.begin();
            }
            assert(invariants_());
            MSS_END();
        }

        bool add_edge(Vertex *src, Vertex *dst)
        {
            MSS_BEGIN(bool, "");
            L(C(src)C(dst));
            assert(invariants_());

            MSS(src != dst);

            auto dst_it = info_.find(dst);
            if (dst_it == info_.end())
            {
                MSS(add_vertex(dst));
                dst_it = info_.find(dst);
            }

            auto src_it = info_.find(src);
            if (src_it == info_.end())
            {
                MSS(add_vertex(src));
                src_it = info_.find(src);
            }

            if (src_it->second.order < dst_it->second.order)
            {
                L("Nodes are correctly ordered");
                MSS(src_it->second.dsts.insert(dst).second);
            }
            else
            {
                L("Dst must be moved after src");
                //Check if all dsts are beyond the src order we have to mode dst to
                {
                    const auto src_order = src_it->second.order;
                    for (auto v: dst_it->second.dsts)
                    {
                        L("Checking dependency on " << C(v));
                        MSS(src_order < info_[v].order);
                    }
                }
                
                {
                    //Remove dst and record the iterator after it: we have to update the order starting from there lateron
                    auto start_it = sequence_.erase(dst_it->second.it);
                    //Get the iterator after the location where dst should be inserted
                    auto it = src_it->second.it; ++it;
                    //Insert dst
                    auto new_dst_it = sequence_.insert(it, dst);
                    //Update the dst info
                    dst_it->second.it = new_dst_it;
                    dst_it->second.order = src_it->second.order;
                    //Update the order for the vertices affected by this move in the sequence
                    //This is expensive since we have to look-up each info in the map
                    for (auto it = start_it; it != new_dst_it; ++it)
                        --info_[*it].order;
                    //Add the edge from src to dst
                    MSS(src_it->second.dsts.insert(dst).second);
                }
            }
            assert(invariants_());
            MSS_END();
        }

        template <Direction dir, typename Ftor>
        bool each_vertex(Ftor ftor)
        {
            MSS_BEGIN(bool);
            switch (dir)
            {
                case Direction::Forward:
                    for (auto v: sequence_) { MSS(ftor(*v)); }
                    break;
                case Direction::Backward:
                    for (auto it = sequence_.rbegin(); it != sequence_.rend(); ++it)
                    {
                        MSS(ftor(**it));
                    }
                    break;
            }
            MSS_END();
        }
        template <Direction dir, typename Ftor>
        bool each_vertex(Ftor ftor) const
        {
            MSS_BEGIN(bool);
            switch (dir)
            {
                case Direction::Forward:
                    for (auto v: sequence_) { MSS(ftor(*v)); }
                    break;
                case Direction::Backward:
                    for (auto it = sequence_.rbegin(); it != sequence_.rend(); ++it)
                    {
                        MSS(ftor(**it));
                    }
                    break;
            }
            MSS_END();
        }

        template <typename Ftor>
        bool each_out(const Vertex *v, Ftor ftor) const
        {
            MSS_BEGIN(bool);
            auto p = info_.find(v);
            MSS(p != info_.end());
            for (auto dst: p->second.dsts)
            {
                MSS(ftor(*dst));
            }
            MSS_END();
        }

        template <Direction dir, typename Ftor>
        bool each_edge(Ftor ftor) const
        {
            auto notify_edges = [&](auto &src){
                auto notify_edge = [&](auto &dst){
                    return ftor(src, dst);
                };
                return each_out(&src, notify_edge);
            };
            return each_vertex<dir>(notify_edges);
        }

        bool remove_unreachables(Vertex *v)
        {
            MSS_BEGIN(bool, "");
            MSS(info_.count(v) > 0);

            Vertices pruned;
            Vertices stage = {v}, new_stage;

            while (!stage.empty())
            {
                assert(new_stage.empty());

                for (auto src: stage)
                    for (auto dst: info_[src].dsts)
                    {
                        MSS(pruned.count(dst) == 0, L("Detected upstream edge: " << *src << " -> " << *dst << std::endl));
                        if (stage.count(dst) == 0)
                            //handle dst in the next iteration
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

            int order = -(int)pruned.size();
            for (auto p = info_.begin(); p != info_.end();)
            {
                if (pruned.count(p->first) > 0)
                {
                    //This vertex should be kept, we only need to update the order
                    p->second.order = ++order;
                    ++p;
                }
                else
                {
                    //This vertex should be removed
                    sequence_.erase(p->second.it);
                    p = info_.erase(p); 
                }
            }

            MSS_END();
        }

        template <typename Ftor>
        void stream(std::ostream &os, Ftor ftor) const
        {
            unsigned int order = 0;
            for (auto v: sequence_)
            {
                os << order << "\t(" << v << ")\t" << ftor(*v) << std::endl;
                auto p = info_.find(v);
                if (p != info_.end())
                    for (auto d: p->second.dsts)
                        os << "\t => " << ftor(*d) << std::endl;
                ++order;
            }
        }

    private:
        bool invariants_() const
        {
            MSS_BEGIN(bool);
            MSS(sequence_.size() == info_.size());
            for (const auto &p: info_)
            {
                MSS(p.first == *p.second.it);
            }
            int order = sequence_.size();
            order = -order;
            for (auto it = sequence_.begin(); it != sequence_.end(); ++it)
            {
                L(**it);
                ++order;
                auto p = info_.find(*it);
                MSS(p != info_.end());
                L(C(p->second.order)C(order));
                MSS(p->second.order == order);
                MSS(p->second.it == it);
            }
            MSS_END();
        }

        Sequence sequence_;
        InfoPerVertex info_;
    };

} } 

#endif
