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

        milliseconds duration = duration_cast<milliseconds>(now-m_startTime);
        return duration.count() / 10.0f;
    }
}
