#ifndef HEADER_gubg_gp_Word_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_Word_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include "gubg/zip.hpp"
#include "gubg/mss.hpp"
#include "gubg/debug.hpp"
#include <vector>
#include <random>
#include <memory>
#include <cassert>

namespace gubg { namespace gp { 

    template <typename Creature, typename Operations>
    class World
    {
    private:
        static constexpr const char *logns = nullptr;//"World";

    public:
        Operations operations;

        World() {}
        World(const Operations &operations): operations(operations) {}

        void resize(size_t size)
        {
            S("resize");L(C(size));
            population_.resize(size);
            infos_.resize(size);
            generation_ = -1;
        }

        const Creature *best() const {return best_.get();}

        bool process()
        {
            MSS_BEGIN(bool, logns);

            ++generation_;

            L("Processing " << C(generation_));

            if (generation_ == 0)
            {
                L("Create initial population, if needed");
                for (auto &creature: population_)
                {
                    MSS(operations.create(creature));
                }
            }

            L("Process the population");
            MSS(operations.process(population_));

            L("Compute the scores for all");
            {
                auto compute_and_set_score = [&](Creature &creature, Info &info)
                {
                    info.creature = &creature;
                    return operations.score(info.score, creature);
                };
                MSS(gubg::zip(RANGE(population_), infos_.begin(), compute_and_set_score));
            }

            L("Kill the weak");
            {
                std::sort(RANGE(infos_), [](const Info &a, const Info &b) { return a.score < b.score;});
                for (auto &info: infos_)
                    info.alive = true;

                auto nr_to_kill = operations.kill_fraction()*population_.size();
                //Make sure we do not kill too much: else, mating becomes difficult
                MSS(nr_to_kill < population_.size());
                for (; nr_to_kill > 0; )
                {
                    /* L(C(nr_to_kill)); */
                    const auto kill_ix = geometric_(rng_);
                    if (kill_ix >= infos_.size())
                    {
                        L("kill_ix too large");
                        continue;
                    }

                    bool killed = false;
                    if (!killed)
                        for (int ix = kill_ix; ix >= 0; --ix)
                        {
                            auto &info = infos_[ix];
                            if (info.alive)
                            {
                                L("Killing " << C(ix));
                                killed = true;
                                info.alive = false;
                                break;
                            }
                        }
                    if (!killed)
                        for (int ix = kill_ix; ix < population_.size(); ++ix)
                        {
                            auto &info = infos_[ix];
                            if (info.alive)
                            {
                                L("Killing " << C(ix));
                                killed = true;
                                info.alive = false;
                                break;
                            }
                        }
                    if (killed)
                        --nr_to_kill;
                }
            }

            L("Mate");
            for (auto &info: infos_)
            {
                if (info.alive)
                    break;
                //This is a dead creature, replace it with a new one
                const Creature *parent_a = nullptr;
                const Creature *parent_b = nullptr;
                while (!parent_a || !parent_b)
                {
                    const auto rnd = geometric_(rng_);
                    if (rnd >= infos_.size())
                        continue;
                    const auto ix = infos_.size() - 1 - rnd;
                    const auto &inf = infos_[ix];
                    if (inf.alive)
                    {
                        assert(!!inf.creature);
                        if (!parent_a)
                            parent_a = inf.creature;
                        else
                            parent_b = inf.creature;
                    }
                }
                assert(!!info.creature);
                MSS(operations.mate(*info.creature, *parent_a, *parent_b));
                info.alive = true;
            }

            //Sort again
            std::sort(RANGE(infos_), [](const Info &a, const Info &b) { return a.score < b.score;});

            const auto &best_info = infos_.back();
            if (!best_ || (!!best_info.creature && best_info.score > best_score_))
            {
                best_.reset(new Creature(*best_info.creature));
                best_score_ = best_info.score;
            }

            MSS_END();
        }

    private:
        using Population = std::vector<Creature>;
        Population population_;

        struct Info
        {
            Creature *creature = nullptr;
            double score = 0.0;
            bool alive = false;
        };
        using Infos = std::vector<Info>;
        Infos infos_;

        int generation_ = -1;

        std::unique_ptr<Creature> best_;
        double best_score_;

        std::mt19937 rng_;
        std::geometric_distribution<> geometric_{0.5};
    };

} } 

#endif
