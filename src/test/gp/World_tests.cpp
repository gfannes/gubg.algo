#include "catch.hpp"
#include "gubg/gp/World.hpp"

namespace  { 
    
    class Creature
    {
        public:
    };
    
    class Operations
    {
        public:
            double kill_fraction_ = 0.1;
            double kill_fraction() const {return kill_fraction_;}

            bool create(Creature &creature) const
            {
                MSS_BEGIN(bool);
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
                MSS_BEGIN(bool);
                MSS_END();
            }
    };

    using World = gubg::gp::World<Creature, Operations>;
} 

TEST_CASE("gp::World tests", "[ut][gp]")
{
    const Operations oper;
    World world(oper);
    world.resize(100);
    for (int i = 0; i < 100; ++i)
    {
        REQUIRE(world.process());
    }
}
