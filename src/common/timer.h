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
        std::chrono::high_resolution_clock::time_point startTime;
    };
}
