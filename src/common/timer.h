#pragma once

namespace rade
{
    class timer
    {
    public:
        timer();
        ~timer();

        void Start();

        double ElapsedTime();

    private:
        double m_start_time;
    };
}
