#include <gubg/signal/LinearChirp.hpp>
#include <gubg/fir/invert.hpp>
#include <gubg/fir/Filter.hpp>
#include <gubg/biquad/Filter.hpp>
#include <gubg/biquad/Tuner.hpp>
#include <gubg/wav/Writer.hpp>
#include <gubg/wav/clamp.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("signal::LinearChirp create WAVE file", "[ut][signal][LinearChirp][wav]")
{
    const double duration_sec = 3.0;
    const double samplerate = 48000;
    gubg::signal::LinearChirp<double> chirp{0.1, 0.0, 20.0, 20000.0, duration_sec};
    gubg::wav::Writer writer;
    writer.open("linear_chirp.wav", 1, 1, samplerate, 24);

    const unsigned int duration_samples = duration_sec*samplerate;
    for (unsigned int six = 0; six < duration_samples; ++six)
    {
        const double t = six/samplerate;
        const float flt = chirp(t)*(1<<23);
        REQUIRE(writer.write_mono(&flt));
    }
}

TEST_CASE("signal::LinearChirp invert tests", "[ut][signal][LinearChirp][invert]")
{
    const double amplitude = 0.1;
    const double duration_sec = 1;
    const double samplerate = 48000;
    gubg::signal::LinearChirp<double> chirp{amplitude, 0.0, 0.0, 24000.0, duration_sec};

    const unsigned int duration_samples = duration_sec*samplerate;
    std::vector<double> signal(duration_samples);
    for (unsigned int six = 0; six < duration_samples; ++six)
    {
        const double t = six/samplerate;
        signal[six] = chirp(t);
    }

    std::vector<double> inv_signal;
    gubg::fir::reverse_frequency(inv_signal, signal);
    for (auto &v: inv_signal)
        v /= inv_signal.size()*0.5*amplitude;

    gubg::fir::Filter<double> inv_chirp{inv_signal};

    gubg::biquad::Filter<double> bp_a, bp_b;
    {
        using Tuner = gubg::biquad::Tuner<double>;
        Tuner tuner{48000};
        tuner.configure(1000, 1, gubg::biquad::Type::Peak);
        tuner.set_gain_db(6);
        const auto ptr = tuner.compute();
        REQUIRE(!!ptr);
        const auto &coeffs = *ptr;
        std::cout << coeffs << std::endl;
        bp_a.set(coeffs);
        bp_b.set(coeffs);
    }

    gubg::wav::Writer writer;
    writer.open("inv_chirp.wav", 1, 5, samplerate, 24);
    std::vector<float> sample;
    for (auto six = 0; six < 3*duration_samples; ++six)
    {
        const auto impulse = (six == duration_samples ? amplitude : 0.0);
        const auto bp_impulse = bp_a(impulse);

        auto go0 = [&](const auto &array){return six < array.size() ? array[six] : 0.0;};
        const auto chirp = go0(signal);
        const auto bp_chirp = bp_b(chirp);

        sample.clear();
        sample.push_back(chirp*(1<<23));
        sample.push_back(impulse*(1<<23));
        sample.push_back(bp_impulse*(1<<23));
        sample.push_back(bp_chirp*(1<<23));
        sample.push_back(inv_chirp(bp_chirp)*(1<<23));
        REQUIRE(writer.write_block([&](auto chix){return sample.data()+chix;}));
    }
}
