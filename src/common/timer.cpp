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
        m_startTime = std::chrono::steady_clock::now();
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime() const
    {
        std::chrono::steady_clock::time_point now = steady_clock::now();

        duration<float> time_span = duration_cast<duration<float>>(now - m_startTime);
        return time_span.count() * 100;
    }
}
