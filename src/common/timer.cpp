#include "timer.h"

namespace rade
{
    timer::timer()
    {
        //m_plfTimer = new plf::nanotimer();
        Start();
    }

    timer::~timer()
    {
        Start();
    }

    void timer::Start()
    {
        m_plfTimer.start();
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime()
    {
        return static_cast<float>(m_plfTimer.get_elapsed_ms());
    }
}
