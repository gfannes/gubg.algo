#include <gubg/signal/LinearChirp.hpp>
#include <gubg/fir/invert.hpp>
#include <gubg/fir/Filter.hpp>
#include <gubg/wav/Writer.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("signal::LinearChirp create WAVE file", "[ut][signal][LinearChirp][wav]")
{
    const double duration_sec = 3.0;
    const double samplerate = 48000;
    gubg::signal::LinearChirp<double> chirp{0.1, 0.0, 20.0, 20000.0, duration_sec};
    gubg::wav::Writer writer{"linear_chirp.wav", 1, samplerate};

    const unsigned int duration_samples = duration_sec*samplerate;
    for (unsigned int six = 0; six < duration_samples; ++six)
    {
        const double t = six/samplerate;
        REQUIRE(writer.add_value(chirp(t)));
    }
}

TEST_CASE("signal::LinearChirp invert tests", "[ut][signal][LinearChirp][invert]")
{
    const double duration_sec = 0.1;
    const double samplerate = 48000;
    gubg::signal::LinearChirp<double> chirp{0.1, 0.0, 20.0, 2000.0, duration_sec};

    const unsigned int duration_samples = duration_sec*samplerate;
    std::vector<double> signal(duration_samples);
    for (unsigned int six = 0; six < duration_samples; ++six)
    {
        const double t = six/samplerate;
        signal[six] = chirp(t);
    }

    std::vector<double> inv_signal;
    gubg::fir::reverse_frequency(inv_signal, signal);

    gubg::fir::Filter<double> inv_chirp{inv_signal};
    gubg::wav::Writer writer{"inv_chirp.wav", 1, samplerate};
    for (auto six = 0; six < 3*duration_samples; ++six)
    {
        const auto v = (six < signal.size() ? signal[six] : 0);
        REQUIRE(writer.add_value(inv_chirp(v)));
    }
}
