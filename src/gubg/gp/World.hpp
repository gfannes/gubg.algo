#ifndef HEADER_gubg_gp_World_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_World_hpp_ALREADY_INCLUDED

#include <gubg/tree/stream.hpp>
#include <gubg/prob/Uniform.hpp>
#include <gubg/mss.hpp>
#include <gubg/Range.hpp>
#include <gubg/stat/Boxplot.hpp>
#include <vector>
#include <random>
#include <algorithm>

namespace gubg { namespace gp { 

    template <typename Policy>
    class World: public Policy
    {
    public:
        bool do_log = true;

        bool process()
        {
            MSS_BEGIN(bool);

            population_.resize(Policy::population_size(generation_));

            MSS(create_initial_genos_());

            MSS(spawn_dead_phenos_());

            MSS(score_());

            MSS(create_mate_pool_());
            MSS(replace_unselected_entities_());

            ++generation_;

            MSS_END();
        }

    private:
        using Geno = typename Policy::Geno;
        using Pheno = typename Policy::Pheno;

        bool create_initial_genos_()
        {
            MSS_BEGIN(bool);
            for (auto &entity: population_)
                if (entity.geno.empty())
                {
                    MSS(Policy::grow(entity.geno));
                    if (do_log)
                    {
                        std::cout << "Created new geno:" << std::endl;
                        gubg::tree::stream(std::cout, entity.geno);
                    }
                }
            MSS_END();
        }
        bool spawn_dead_phenos_()
        {
            MSS_BEGIN(bool);
            for (auto &entity: population_)
                if (entity.selection_count == 0)
                    MSS(Policy::spawn(entity.pheno, entity.geno));
            MSS_END();
        }
        bool score_()
        {
            MSS_BEGIN(bool);

            const auto size = population_.size();
            genos_.resize(size);
            phenos_.resize(size);
            scores_.resize(size);

            for (auto ix = 0u; ix < size; ++ix)
            {
                auto &entity = population_[ix];
                genos_[ix] = &entity.geno;
                phenos_[ix] = &entity.pheno;
                scores_[ix] = &entity.score;
            }

            MSS(Policy::score(scores_, genos_, phenos_, generation_));

            if (false)
            {
                score_boxplot_.reset();
                for (auto &entity: population_)
                    score_boxplot_ << entity.score;
                score_boxplot_.calculate().stream(std::cout);
            }

            MSS_END();
        }
        bool create_mate_pool_()
        {
            MSS_BEGIN(bool);

            const auto size = population_.size();
            ordered_population_.resize(size);
            for (auto ix = 0u; ix < size; ++ix)
            {
                auto &entity = population_[ix];
                entity.selection_count = 0;
                ordered_population_[ix] = &entity;
            }

            std::sort(RANGE(ordered_population_), [](Entity *a, Entity *b) { return b->score < a->score;});

            mate_pool_.resize(size);
            /* const double p = 1.0-std::pow(1.0/std::log(size), 1.0/size); */
            const double p = 1.0-std::pow(1.0/20.0, 1.0/size);
            std::geometric_distribution<> geometric{p};
            for (auto ix = 0u; ix < size; ++ix)
            {
                while (true)
                {
                    const size_t selected_ix = geometric(rng_);
                    if (selected_ix < size)
                    {
                        Entity *selected_entity = ordered_population_[selected_ix];
                        ++selected_entity->selection_count;
                        mate_pool_[ix] = selected_entity;
                        break;
                    }
                }
            }

            MSS_END();
        }
        bool replace_unselected_entities_()
        {
            MSS_BEGIN(bool);

            for (auto &entity: population_)
                if (entity.selection_count == 0)
                {
                    Entity *a = gubg::prob::select_uniform(mate_pool_, rng_);
                    Entity *b = gubg::prob::select_uniform(mate_pool_, rng_);
                    MSS(Policy::procreate(entity.geno, a->geno, b->geno));
                }

            MSS_END();
        }

        unsigned int generation_ = 0;

        struct Entity
        {
            Geno geno{};
            Pheno pheno{};
            double score{};
            unsigned int selection_count{};
        };
        using Population = std::vector<Entity>;
        Population population_;

        std::vector<Entity *> ordered_population_;
        std::vector<Entity *> mate_pool_;

        std::vector<const Geno *> genos_;
        std::vector<const Pheno *> phenos_;
        std::vector<double *> scores_;

        std::mt19937 rng_;

        gubg::stat::Boxplot score_boxplot_;
    };

} } 

#endif
