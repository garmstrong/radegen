#pragma once

#include <chrono>
namespace rade
{
    class Timer
    {
    public:
        Timer();

        void Start();

        float ElapsedTime() const;

    private:
        std::chrono::high_resolution_clock::time_point startTime;
    };
}
