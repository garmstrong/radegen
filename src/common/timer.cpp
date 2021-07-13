#include "timer.h"

using namespace std::chrono;

namespace rade
{
    timer::timer()
    {
        Start();
    }

    void timer::Start()
    {
        startTime = high_resolution_clock::now();
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime() const
    {
        high_resolution_clock::time_point now = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(now - startTime);
        return static_cast<float>(time_span.count());
    }
}
