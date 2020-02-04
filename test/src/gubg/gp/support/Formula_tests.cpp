#include <gubg/gp/support/Formula.hpp>
#include <gubg/gp/W2.hpp>
#include <gubg/tree/Forest.hpp>
#include <catch.hpp>

TEST_CASE("gp::support::Formula geno tests", "[gp][support][geno][Formula]")
{
    using namespace support::geno;
    using Forest = gubg::tree::Forest<Node>;
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

        size_t population_size(unsigned int generation) const {return 100;}
        bool grow(Geno &geno)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        bool spawn(Pheno &pheno, const Geno &geno)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        bool score(double &s, Pheno &pheno)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
        bool procreate(Geno &child, const Geno &a, const Geno &b)
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
    private:
    };
} 

TEST_CASE("gp::W2 tests", "[gp][W2]")
{
    using World = gubg::gp::W2<MyPolicy>;
    World world;
    for (auto ix = 0u; ix < 1; ++ix)
    {
        REQUIRE(world.process());
    }
}
