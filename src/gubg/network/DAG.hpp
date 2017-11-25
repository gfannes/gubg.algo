#ifndef HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_network_DAG_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <set>
#include <map>
#include <vector>
#include <ostream>
#include <cassert>

namespace gubg { namespace network { 

    enum class Direction { Forward, Backward, };
    enum class State {Unvisited, Visiting, Visited};

    template <typename Vertex>
    class DAG
    {
    public:
        using Self = DAG<Vertex>;
        using Set = std::set<Vertex*>;
        using Vertices = std::set<const Vertex*>;
        struct Info
        {
            size_t order = 0;
            Set dsts;
            State state;
            Info(){}
            Info(size_t order): order(order){}
        };
        using InfoPerVertex = std::map<Vertex*, Info>;
        using Sequence = std::vector<typename InfoPerVertex::iterator>;

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
            auto order = size();
            auto it = info_.insert(std::make_pair(v, Info{order}));
            if (!it.second)
            {
                //Node is already present
                MSS(!fail_when_present);
            }
            else
            {
                //Node should be added
                sequence_.push_back(it.first);
            }
            assert(invariants_());
            MSS_END();
        }

        bool add_edge(Vertex *src, Vertex *dst)
        {
            MSS_BEGIN(bool, "");
            L(C(*src)C(*dst));
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

            //Insert the edge
            auto p = src_it->second.dsts.insert(dst);
            MSS(p.second);

            if (src_it->second.order > dst_it->second.order)
                //Node are not correcly ordered: sort all the nodes
                //When this fails, we have a directed cycle and will remove the already added edge again
                MSS(sort_(), src_it->second.dsts.erase(p.first));

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
                    for (const auto &it: sequence_) { MSS(ftor(*it->first)); }
                    break;
                case Direction::Backward:
                    for (auto it = sequence_.rbegin(); it != sequence_.rend(); ++it)
                    {
                        MSS(ftor(*(*it)->first));
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
                    for (const auto &it: sequence_) { MSS(ftor(*it->first)); }
                    break;
                case Direction::Backward:
                    for (auto it = sequence_.rbegin(); it != sequence_.rend(); ++it)
                    {
                        MSS(ftor(*(*it)->first));
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

            Set pruned;
            Set stage = {v}, new_stage;

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

            db_seq_.resize(pruned.size());
            db_seq_ix_ = db_seq_.size()-1;

            for (size_t src_ix = sequence_.size(); src_ix-- > 0;)
            {
                const auto &it = sequence_[src_ix];
                if (pruned.count(it->first) > 0)
                {
                    //This vertex should be kept
                    it->second.order = db_seq_ix_;
                    db_seq_[db_seq_ix_--] = it;
                }
                else
                {
                    //This vertex should be removed
                    info_.erase(it); 
                }
            }

            db_seq_.swap(sequence_);

            MSS_END();
        }

        template <typename Ftor>
        void stream(std::ostream &os, Ftor ftor) const
        {
            unsigned int order = 0;
            for (const auto &it: sequence_)
            {
                assert(it != info_.end());
                {
                    auto v = it->first;
                    os << order << "\t(" << v << ")\t" << ftor(*v) << std::endl;
                }
                for (auto d: it->second.dsts)
                    os << "\t => " << ftor(*d) << std::endl;
                ++order;
            }
        }

    private:
        bool visit_(const typename InfoPerVertex::iterator &it)
        {
            MSS_BEGIN(bool, "");
            L(*it->first);
            MSS(it->second.state != State::Visiting);
            if (it->second.state == State::Unvisited)
            {
                it->second.state = State::Visiting;
                for (auto v: it->second.dsts)
                {
                    MSS(visit_(info_.find(v)));
                }
                {
                    it->second.order = db_seq_ix_;
                    it->second.state = State::Visited;
                    db_seq_[db_seq_ix_--] = it;
                }
            }
            MSS_END();
        }
        bool sort_()
        {
            MSS_BEGIN(bool, "");

            for (auto &p: info_)
                p.second.state = State::Unvisited;

            db_seq_.resize(sequence_.size());
            db_seq_ix_ = db_seq_.size()-1;
            for (size_t src_ix = sequence_.size(); src_ix-- > 0;)
            {
                MSS(visit_(sequence_[src_ix]));
            }

            db_seq_.swap(sequence_);

            MSS_END();
        }
        bool invariants_() const
        {
            MSS_BEGIN(bool, "");
            MSS(sequence_.size() == info_.size());
            for (size_t ix = 0; ix < sequence_.size(); ++ix)
            {
                const auto it = sequence_[ix];
                MSS(it != info_.end());
                MSS(!!it->first);
                MSS(it->second.order == ix);
                for (auto v: it->second.dsts)
                {
                    auto p = info_.find(v);
                    MSS(p != info_.end());
                    MSS(it->second.order < p->second.order, std::cout << "Error: This is not a DAG" << std::endl);
                }
            }
            MSS_END();
        }

        Sequence sequence_;
        Sequence db_seq_;
        size_t db_seq_ix_;
        InfoPerVertex info_;
    };

} } 

#endif
