#pragma once

#include <chrono>
namespace rade
{
    class timer
    {
    public:
        timer();

        void Start();

        float ElapsedTime() const;

    private:
        //std::chrono::high_resolution_clock::time_point m_startTime;
        std::chrono::steady_clock::time_point m_startTime;
    };
}
