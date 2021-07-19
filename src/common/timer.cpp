#include "timer.h"
#include "osutils.h"

namespace rade
{
    timer::timer()
    {
        Start();
    }

    void timer::Start()
    {
        m_startTime = rade::GetTimer();
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime() const
    {
        double endTime = rade::GetTimer();
        double elapsedSec = (endTime - m_startTime) / rade::GetTimerFrequency();
        return static_cast<float>(elapsedSec);
    }
}
