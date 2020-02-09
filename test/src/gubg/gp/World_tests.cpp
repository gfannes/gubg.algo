#include <gubg/gp/support/Formula.hpp>
#include <gubg/gp/support/Policy.hpp>
#include <gubg/gp/World.hpp>
#include <catch.hpp>
#include <optional>

TEST_CASE("gp::World tests", "[gp][World]")
{
    using World = gubg::gp::World<support::Policy>;
    World world;
    for (auto ix = 0u; ix < 100; ++ix)
    {
        REQUIRE(world.process());
    }
}
