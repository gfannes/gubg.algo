#include "catch.hpp"
#include "gubg/pid/Controller.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

namespace  { 
    using T = double;

    class Rocket: public pid::Controller<Rocket, T>
    {
    public:
        void pid_coefs(T &p, T &i, T &d, T time) const
        {
            p = 10.0;
            i = 5.0;
            d = 10.0;
        }

        T pid_simulate(T time, T dt, T u)
        {
            v += (u-g_)/mass_*dt;
            x += v*dt;
            return (target_x_ - x);
        }

    public:
        T x{};
        T v{};

    private:
        const T g_ = 9.81;
        T mass_ = 1.0;
        T target_x_ = 1.0;
    };
} 

TEST_CASE("gubg::pid::Controller tests", "[ut][gubg][pid]")
{
    Rocket rocket;
    const T dt = 0.001;
    for (; rocket.time() < 10; rocket.process(dt))
    {
        cout << "[rocket](time:" << rocket.time() << ")(x:" << rocket.x << ")(v:" << rocket.v << "){}" << endl;
    }
}
