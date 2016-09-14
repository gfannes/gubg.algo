#include "catch.hpp"
#include "gubg/gp/World.hpp"
#include <cmath>
#include <random>
#include <algorithm>
#include <deque>
#include <ostream>
#include <fstream>

namespace  { 
    const double pipi = 2.0*3.1415926;

    using Signal = std::deque<double>;
    
    class Creature
    {
        public:
            Signal signal;

            double interpolate(double d) const
            {
                const auto size = signal.size();
                if (size == 0)
                    return 0.0;
                if (size == 1)
                    return signal[0];

                const auto cix_c = d*double(size);
                size_t ix_c = std::floor(cix_c); 
                if (ix_c >= size-1)
                    return signal.back();

                const double dd = (cix_c - double(ix_c));
                const auto low = signal[ix_c];
                const auto heigh = signal[ix_c+1];
                return (1.0-dd)*low + dd*heigh;
            }
    };
    std::ostream & operator<<(std::ostream &os, const Creature &creature)
    {
        for (auto s: creature.signal)
            os << C(s);
        return os;
    }

    class Operations
    {
        public:
            Operations() {}
            Operations(const Operations &rhs): signal_(rhs.signal_) { }
            Operations(const Signal &signal): signal_(signal) { }

            double kill_fraction_ = 0.1;
            double kill_fraction() const {return kill_fraction_;}

            bool create(Creature &creature) const
            {
                MSS_BEGIN(bool);
                creature.signal.clear();
                MSS_END();
            }

            template <typename Population>
                bool process(Population &population) const
                {
                    MSS_BEGIN(bool);
                    MSS_END();
                }

            bool score(double &score, const Creature &creature) const
            {
                MSS_BEGIN(bool, "test");
                double cost = 0.0;
                const auto size = signal_.size();
                const auto size_c = creature.signal.size();
                L(C(size_c));
                /* cost += double(size_c)/double(size*100); */
                for (size_t ix = 0; ix < size; ++ix)
                {
                    double interpolated = creature.interpolate(double(ix)/double(size));

                    const auto diff = (signal_[ix] - interpolated);
                    cost += diff*diff;
                    L(C(ix)C(interpolated)C(signal_[ix])C(diff)C(cost));
                }
                score = -cost;
                L(C(cost)C(score));
                MSS_END();
            }

            bool mate(Creature &child, const Creature &a, const Creature &b) const
            {
                MSS_BEGIN(bool);
                child.signal.resize(0);
                const double d = uniform_(rng_);
                {
                    const size_t nr_a = d*a.signal.size();
                    child.signal.insert(child.signal.end(), a.signal.begin(), a.signal.begin()+nr_a);
                }
                {
                    const size_t nr_b = d*b.signal.size();
                    child.signal.insert(child.signal.end(), b.signal.begin(), b.signal.begin()+nr_b);
                }

                for (unsigned int nr_to_add = geometric_(rng_); nr_to_add > 0; --nr_to_add)
                    child.signal.push_back(noise_(rng_));
                for (unsigned int nr_to_remove = std::min<unsigned int>(geometric_(rng_), child.signal.size()); nr_to_remove > 0; --nr_to_remove)
                    child.signal.pop_front();

                MSS_END();
            }

        private:
            Signal signal_;
            mutable std::mt19937 rng_;
            mutable std::uniform_real_distribution<> uniform_{0.0, 1.0};
            mutable std::geometric_distribution<> geometric_{0.5};
            mutable std::uniform_real_distribution<> noise_{-1.0, 1.0};
    };

    using World = gubg::gp::World<Creature, Operations>;

    bool create_sine(Signal &signal, size_t size)
    {
        MSS_BEGIN(bool);
        signal.resize(size);
        double t = 0.0;
        const double step = pipi/double(size);
        for (auto &s: signal)
        {
            s = std::sin(t);
            t += step;
        }
        MSS_END();
    }
} 

TEST_CASE("gp::World tests", "[ut][gp]")
{
    S("test");
    Signal signal;
    REQUIRE(create_sine(signal, 100));
    const Operations oper(signal);
    World world(oper);
    world.resize(100);
    for (int i = 0; i < 10000; ++i)
    {
        REQUIRE(world.process());
    }

    REQUIRE(!!world.best());
    const auto &best = *world.best();

    L(C(best));
    std::ofstream of("output.txt", std::ios::out);
    of << "$data << EOD" << std::endl;
    unsigned int ix = 0;
    for (double d = 0.0; d < 1.0; d += 0.1, ++ix)
    {
        of << ix << ' ' << d << ' ' << best.interpolate(d) << std::endl;
    }
    of << "EOD" << std::endl;
    of << "plot $data using 2:3" << std::endl;
    of << "pause mouse" << std::endl;
}
