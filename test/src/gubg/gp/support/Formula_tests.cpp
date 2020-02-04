#include <gubg/gp/support/Formula.hpp>
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
