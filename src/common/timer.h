#pragma once
#include <cstdint>

namespace rade
{
    class timer
    {
     public:
        timer();

        void Start();

        float ElapsedTime() const;

     private:
        double m_startTime = 0;
    };
}
