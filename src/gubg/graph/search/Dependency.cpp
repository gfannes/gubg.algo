#include <gubg/graph/search/Dependency.hpp>
#include <gubg/hr.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace graph { namespace search {

    bool Dependency::init(const Graph *g)
    {
        MSS_BEGIN(bool);

        MSS(!!g);
        g_ = g;

        vertex__incount_.resize(g_->vertex_count());

        reset();

        MSS_END();
    }

    void Dependency::reset()
    {
        std::fill(RANGE(vertex__incount_), 0);
        unlocked_.resize(0);
        unlocked_tmp_.resize(0);

        const auto v_count = g_->vertex_count();
        for (Vertex v = 0; v < v_count; ++v)
        {
            if (is_unlocked_(v))
                unlocked_tmp_.push_back(v);
        }

        processing_count_ = 0;
    }

    bool Dependency::topo_order(Vertices &order)
    {
        order.resize(0);
        return each([&](Vertex v) { order.push_back(v); });
    }

    bool Dependency::next(Vertex &v)
    {
        if (unlocked_.empty())
        {
            if (unlocked_tmp_.empty())
                return false;
            unlocked_.swap(unlocked_tmp_);
        }

        v = unlocked_.back();
        unlocked_.pop_back();

        g_->each_out(v, [&](auto v_dst) {
            ++vertex__incount_[v_dst];
            if (is_unlocked_(v_dst))
                unlocked_tmp_.push_back(v_dst);
        });

        return true;
    }

    bool Dependency::next_mt(Vertex_opt &v)
    {
        std::unique_lock<std::mutex> lock{mutex_};

        assert(valid());

        if (unlocked_.empty())
        {
            if (unlocked_tmp_.empty())
            {
                v.reset();
                const bool still_processing = (processing_count_ > 0);
                return still_processing;
            }
            unlocked_.swap(unlocked_tmp_);
        }

        v = unlocked_.back();
        unlocked_.pop_back();
        ++processing_count_;

        return true;
    }

    void Dependency::done_mt(Vertex v)
    {
        std::unique_lock<std::mutex> lock{mutex_};

        assert(valid());

        g_->each_out(v, [&](auto v_dst) {
            ++vertex__incount_[v_dst];
            if (is_unlocked_(v_dst))
                unlocked_tmp_.push_back(v_dst);
        });

        --processing_count_;
    }

    // Privates
    bool Dependency::is_unlocked_(Vertex v) const
    {
        assert(valid());

        const auto indegree = g_->indegree(v);
        const auto incount = vertex__incount_[v];
        return incount == indegree;
    }

}}} // namespace gubg::graph::search
