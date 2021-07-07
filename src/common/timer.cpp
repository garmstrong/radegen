#include "timer.h"

using namespace std::chrono;

CTimer::CTimer()
{
    Start();
}

void CTimer::Start()
{
    startTime = high_resolution_clock::now();
}

// returns time elapses since start() or constructor called
float CTimer::ElapsedTime() const
{
    high_resolution_clock::time_point now = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(now - startTime);
    return static_cast<float>(time_span.count());
}
