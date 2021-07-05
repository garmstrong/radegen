#pragma once

#include <chrono>

class CTimer
{
public:
    CTimer();

    void Start();

    float ElapsedTime() const;

private:
    std::chrono::high_resolution_clock::time_point startTime;
};
