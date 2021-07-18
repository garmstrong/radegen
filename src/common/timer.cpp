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
        //startTime = high_resolution_clock::now();
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime() const
    {
        //high_resolution_clock::time_point now = high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::milli>(now-m_startTime).count();
        //duration<double> time_span = duration_cast<duration<double>>(now - startTime);
        return static_cast<float>(elapsed_time_ms);
    }
}
