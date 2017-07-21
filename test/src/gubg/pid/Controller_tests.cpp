#include "catch.hpp"
#include "gubg/pid/Controller.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

namespace  { 
    using T = double;

    class Rocket
    {
    public:
        Rocket()
        {
            pid_.p(10.0);
            pid_.i(5.0);
            pid_.d(10.0);
        }

        T time() const {return time_;}

        void process(T dt)
        {
            time_ += dt;

            v += (pid_.u()-g_)/mass_*dt;
            x += v*dt;

            const T error = (target_x_ - x);

            pid_.update(dt, error);
        }

    public:
        T x{};
        T v{};

    private:
        const T g_ = 9.81;
        T mass_ = 1.0;
        T target_x_ = 1.0;

        T time_{};

        pid::Controller<T> pid_;
    };
} 

TEST_CASE("gubg::pid::Controller tests", "[ut][gubg][pid]")
{
    Rocket rocket;
    const T dt = 0.001;
    for (; rocket.time() < 30; rocket.process(dt))
    {
        cout << "[rocket](time:" << rocket.time() << ")(x:" << rocket.x << ")(v:" << rocket.v << "){}" << endl;
    }
}
