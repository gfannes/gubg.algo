#ifndef HEADER_gubg_gp_Word_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_Word_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include "gubg/zip.hpp"
#include "gubg/mss.hpp"
#include "gubg/debug.hpp"
#include <vector>
#include <random>

namespace gubg { namespace gp { 

    template <typename Creature, typename Operations>
        class World
        {
            private:
                static constexpr const char *logns = "World";

            public:
                World(const Operations &operations): operations_(operations) {}

                void resize(size_t size)
                {
                    population_.resize(size);
                    infos_.resize(size);
                    alive_.resize(size);
                    generation_ = -1;
                }

                bool process()
                {
                    MSS_BEGIN(bool, logns);

                    ++generation_;

                    L("Processing " << C(generation_));

                    //Create initial population, if needed
                    if (generation_ == 0)
                    {
                        for (auto &creature: population_)
                        {
                            MSS(operations_.create(creature));
                        }
                    }

                    //Process the population
                    MSS(operations_.process(population_));

                    //Compute the scores for all
                    {
                        auto compute_and_set_score = [&](const Creature &creature, Info &info)
                        {
                            return operations_.score(info.score, creature);
                        };
                        MSS(gubg::zip(RANGE(population_), infos_.begin(), compute_and_set_score));
                    }

                    //Kill the weak
                    {
                        std::fill(RANGE(alive_), true);

                        std::geometric_distribution<> geometric(0.5);

                        auto nr_to_kill = operations_.kill_fraction()*population_.size();
                        for (; nr_to_kill > 0; --nr_to_kill)
                        {
                            const auto kill_ix = geometric(rng_);

                            for (int ix = kill_ix; ix >= 0; --ix)
                            {
                                 if (alive_[ix])
                                 {
                                     alive_[ix] = false;
                                     break;
                                 }
                            }
                            for (int ix = kill_ix; ix < population_.size(); ++ix)
                            {
                                 if (alive_[ix])
                                 {
                                     alive_[ix] = false;
                                     break;
                                 }
                            }
                        }
                    }
                    
                    //Mate

                    MSS_END();
                }

            private:
                const Operations operations_;

                using Population = std::vector<Creature>;
                Population population_;

                struct Info
                {
                    double score = 0.0;
                };
                using Infos = std::vector<Info>;
                Infos infos_;

                using Alive = std::vector<bool>;
                Alive alive_;

                int generation_ = -1;

                std::mt19937 rng_;
        };

} } 

#endif
