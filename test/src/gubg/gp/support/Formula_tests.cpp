#include <gubg/gp/support/Formula.hpp>
#include <gubg/gp/W2.hpp>
#include <gubg/tree/Forest.hpp>
#include <gubg/tree/grow.hpp>
#include <gubg/prob/Bernoulli.hpp>
#include <gubg/prob/Uniform.hpp>
#include <gubg/hr.hpp>
#include <gubg/RMS.hpp>
#include <gubg/wav/Writer.hpp>
#include <catch.hpp>
#include <optional>

TEST_CASE("gp::support::Formula geno tests", "[gp][support][geno][Formula]")
{
    using namespace support::geno;
    Forest forest;
}

TEST_CASE("gp::support::Formula pheno tests", "[gp][support][pheno][Formula]")
{
    using namespace support::pheno;

    Formula formula = {
        Operation::Plus, IX::A,IX::X,
        Operation::Sine, IX::data(0),
    };

    Tape tape;
    REQUIRE(process<true>(tape, formula));

    tape[IX::A] = 2.0;

    for (auto x = -3.0; x <= 3.0; x += 0.1)
    {
        tape[IX::X] = x;
        REQUIRE(process<false>(tape, formula));
        std::cout << x << '\t' << tape.back() << std::endl;
    }
}

namespace  { 
    class MyPolicy
    {
    public:
        using Geno = gubg::tree::Forest<std::string>;
        using Pheno = support::pheno::Formula;

        const static bool seed_with_time = true;

        static void specialize_node(Geno::Node &node, const gubg::tree::Path &path)
        {
            static const std::vector<std::string> functions = {"+", "*", "sin", "cos"};
            static const std::vector<std::string> terminals = {"a", "b", "c", "x"};

            const auto &vec = gubg::prob::choose(functions, terminals, 1.0/path.size(), seed_with_time);
            const auto &str = gubg::prob::select_uniform(vec, seed_with_time);
            node.value = str;
            if (false) {}
            else if (str == "+" || str == "*")     { node.childs.nodes.resize(2); }
            else if (str == "sin" || str == "cos") { node.childs.nodes.resize(1); }
            else                                   { node.childs.nodes.clear(); }
        }


        size_t population_size(unsigned int generation) const {return 1000;}
        bool grow(Geno &geno)
        {
            MSS_BEGIN(bool);
            MSS(geno.empty());
            if (gubg::prob::choose(true, false, 1.0, seed_with_time))
            {
                /* auto specialize_node = [&](auto &node, const auto &path) */
                /* { */
                /*     const auto &vec = gubg::prob::choose(functions, terminals, 1.0/path.size(), seed_with_time); */
                /*     const auto &str = gubg::prob::select_uniform(vec, seed_with_time); */
                /*     node.value = str; */
                /*     if (false) {} */
                /*     else if (str == "+" || str == "*")     { node.childs.nodes.resize(2); } */
                /*     else if (str == "sin" || str == "cos") { node.childs.nodes.resize(1); } */
                /* }; */
                gubg::tree::grow(geno, specialize_node);
            }
            else
            {
                geno.find({0}, true)->value = "*";
                geno.find({0, 0}, true)->value = "a";
                geno.find({0, 1}, true)->value = "sin";
                geno.find({0, 1, 0}, true)->value = "+";
                geno.find({0, 1, 0, 0}, true)->value = "cos";
                geno.find({0, 1, 0, 0, 0}, true)->value = "*";
                geno.find({0, 1, 0, 0, 0, 0}, true)->value = "b";
                geno.find({0, 1, 0, 0, 0, 1}, true)->value = "x";
                geno.find({0, 1, 0, 1}, true)->value = "c";
            }
            MSS_END();
        }
        bool spawn(Pheno &pheno, const Geno &geno)
        {
            MSS_BEGIN(bool);
            pheno.clear();
            size_t data_ix = 0;
            std::vector<int> stack;
            auto move_ixs = [&](size_t nr)
            {
                for (auto ix = 0u; ix < nr; ++ix)
                {
                    pheno.push_back(stack.back());
                    stack.pop_back();
                }
            };
            auto ftor = [&](const auto &node, const auto &path, bool enter)
            {
                if (enter)
                    return;
                const auto &str = node.value;
                if (false) {}
                else if (str == "a") { stack.push_back(support::pheno::IX::A); }
                else if (str == "b") { stack.push_back(support::pheno::IX::B); }
                else if (str == "c") { stack.push_back(support::pheno::IX::C); }
                else if (str == "x") { stack.push_back(support::pheno::IX::X); }
                else
                {
                    if (false) {}
                    else if (str == "+")   { pheno.push_back(support::pheno::Operation::Plus); move_ixs(2); }
                    else if (str == "*")   { pheno.push_back(support::pheno::Operation::Mul);  move_ixs(2); }
                    else if (str == "sin") { pheno.push_back(support::pheno::Operation::Sine); move_ixs(1); }
                    else if (str == "cos") { pheno.push_back(support::pheno::Operation::Cosine);  move_ixs(1); }
                    stack.emplace_back(support::pheno::IX::data(data_ix++));
                }
            };
            geno.dfs(ftor);
            L("Spawned pheno: " << gubg::hr(pheno));
            MSS_END();
        }
        template <typename Scores, typename Genos, typename Phenos>
        bool score(Scores &scores, const Genos &genos, const Phenos &phenos, unsigned int generation)
        {
            MSS_BEGIN(bool);

            auto target_function = [&](auto x)
            {
                return 0.5*std::sin(std::cos(2.0*x)+3.0);
                //This function seems to be fairly close: sin(cos(2.0*cos(2.0)*cos(sin(x))))
            };

            const auto size = scores.size();
            MSS(genos.size() == size);
            MSS(phenos.size() == size);

            gubg::stat::Boxplot fitness_bp;
            gubg::stat::Boxplot complexity_bp;
            gubg::stat::Boxplot score_bp;
            const Geno *best_geno = nullptr;
            const Pheno *best_pheno = nullptr;
            double best_score = 0;
            for (auto ix = 0u; ix < size; ++ix)
            {
                auto &score = *scores[ix];
                const auto &geno = *genos[ix];
                const auto &pheno = *phenos[ix];

                tape_.resize(support::pheno::IX::Nr_);
                tape_[support::pheno::IX::A] = 0.5;
                tape_[support::pheno::IX::B] = 2.0;
                tape_[support::pheno::IX::C] = 3.0;
                support::pheno::process<true>(tape_, pheno);

                gubg::RMS<double> rms;
                for (double x = -3.0; x <= 3.0; x += 0.01)
                {
                    const auto target = target_function(x);
                    tape_[support::pheno::IX::X] = x;
                    support::pheno::process<false>(tape_, pheno);
                    const auto actual = tape_.back();
                    rms << (target-actual);
                }

                const auto fitness = -rms.linear();
                const auto node_count = geno.node_count();
                const auto complexity = - std::log(node_count+1000);
                score = fitness + complexity;

                if (!best_geno || score > best_score)
                {
                    best_geno = &geno;
                    best_pheno = &pheno;
                    best_score = score;
                }

                fitness_bp << fitness;
                complexity_bp << node_count;
                score_bp << score;
            }
            std::cout << std::endl << generation << std::endl;
            fitness_bp.calculate().stream(std::cout << "fitness: ", true);
            complexity_bp.calculate().stream(std::cout << "complexity: ", true);
            score_bp.calculate().stream(std::cout << "score: ", true);
            std::cout << "Best geno: " << best_score << "\n" << support::geno::hr(*best_geno);

            if (!ww_)
                ww_.emplace("output.wav", 2, 48000);

            {
                std::array<double, 2> sample{};
                for (auto ix = 0u; ix < 20; ++ix)
                    ww_->add_sample(sample);

                tape_.resize(support::pheno::IX::Nr_);
                tape_[support::pheno::IX::A] = 2.0;
                tape_[support::pheno::IX::B] = 3.0;
                tape_[support::pheno::IX::C] = 4.0;
                support::pheno::process<true>(tape_, *best_pheno);
                for (double x = -3.0; x <= 3.0; x += 0.01)
                {
                    sample[0] = target_function(x);
                    tape_[support::pheno::IX::X] = x;
                    support::pheno::process<false>(tape_, *best_pheno);
                    sample[1] = tape_.back();
                    ww_->add_sample(sample);
                }
            }

            MSS_END();
        }
        bool procreate(Geno &child, const Geno &a, const Geno &b)
        {
            MSS_BEGIN(bool);

            L("a\n" << support::geno::hr(a));
            L("b\n" << support::geno::hr(b));
            child = a;

            auto random_path = [&](const Geno &geno)
            {
                gubg::prob::Uniform uniform{geno.node_count()};
                const auto rnd = uniform(seed_with_time);
                unsigned int ix = 0;
                gubg::tree::Path my_path;
                auto ftor = [&](const auto &, const auto &path, bool enter)
                {
                    if (!enter)
                        return;
                    if (ix == rnd)
                        my_path = path;
                    ++ix;
                };
                geno.dfs(ftor);
                return my_path;
            };

            const auto prune_path = random_path(child);
            auto prune_node = child.find(prune_path);

            if (gubg::prob::choose(true, false, 0.5, seed_with_time))
            {
                const auto src_path = random_path(b);
                L(C(gubg::hr(prune_path))C(gubg::hr(src_path)));

                *prune_node = *b.find(src_path);
            }
            else
            {
                gubg::tree::Path path = {0};
                gubg::tree::details::grow(*prune_node, path, specialize_node);
            }

            L("child\n" << support::geno::hr(child));

            MSS_END();
        }

    private:
        support::pheno::Tape tape_;
        std::optional<gubg::wav::Writer> ww_;
    };
} 

TEST_CASE("gp::W2 tests", "[gp][W2]")
{
    using World = gubg::gp::W2<MyPolicy>;
    World world;
    for (auto ix = 0u; ix < 100; ++ix)
    {
        REQUIRE(world.process());
    }
}
