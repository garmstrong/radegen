#pragma once
#include "plf_timer.h"

namespace rade
{
    class timer
    {
    public:
        timer();
        ~timer();

        void Start();

        float ElapsedTime();

    private:
        plf::nanotimer m_plfTimer;
    };
}
