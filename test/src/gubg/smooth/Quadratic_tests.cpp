#include <gubg/smooth/Quadratic.hpp>
#include <catch.hpp>
using namespace gubg;

TEST_CASE("smooth::Quadratic tests", "[ut][smooth][Quadratic]")
{
	smooth::Quadratic<float> qs;

	REQUIRE(qs.latency() == 1);

	REQUIRE(qs.value() == Approx(0.0f));
	REQUIRE(qs.derivative() == Approx(0.0f));
	REQUIRE(qs.curvature() == Approx(0.0f));

	const float a = 1;
	const float b = 2;
	const float c = 3;
	auto f = [&](float x){return a*x*x + b*x + c;};

	qs(f(-1));
	qs(f(0));
	qs(f(1));

	REQUIRE(qs.value() == Approx(c));
	REQUIRE(qs.derivative() == Approx(b));
	REQUIRE(qs.curvature() == Approx(2*a));
}	