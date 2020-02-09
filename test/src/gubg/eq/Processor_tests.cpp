#include <gubg/eq/Processor.hpp>
#include <catch.hpp>
#include <array>
#include <iostream>
#include <fstream>

TEST_CASE("eq::Processor tests", "[eq][Processor]")
{
    using Path = gubg::tree::Path;

    gubg::eq::Geno geno;
    geno.find(Path{0}, true)->value = gubg::eq::Mix{};
    geno.find(Path{0,0}, true)->value = gubg::eq::Biquad{0.5,0,0, 0,0};
    geno.find(Path{0,1}, true)->value = gubg::eq::Cascade{};
    geno.find(Path{0,1,0}, true)->value = gubg::eq::Biquad{0.1,0,0, 0,0};
    geno.find(Path{0,1,1}, true)->value = gubg::eq::Delay{3};

    {
        std::ofstream fo("geno.naft");
        REQUIRE(gubg::eq::serialize(fo, geno));
    }
    {
        std::ostringstream oss;
        REQUIRE(gubg::eq::serialize(oss, geno));
        gubg::eq::Geno geno2;
        REQUIRE(gubg::eq::deserialize(geno2, oss.str()));
        std::ostringstream oss2;
        REQUIRE(gubg::eq::serialize(oss2, geno2));
        REQUIRE(oss.str() == oss2.str());
    }

    gubg::eq::Processor<float> processor;
    REQUIRE(processor.initialize(geno, 10));

    std::array<float, 10> buffer{};
    buffer[0] = 1;

    auto ptr = buffer.data();
    REQUIRE(processor.process(ptr, ptr+buffer.size()));

    for (auto ix = 0u; ix < buffer.size(); ++ix)
        std::cout << ix << ": " << buffer[ix] << std::endl;
}
