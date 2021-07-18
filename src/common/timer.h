#pragma once

namespace rade
{
    class timer
    {
    public:
        timer();

        void Start();

        float ElapsedTime() const;

    private:
        float m_start_time;
    };
}
