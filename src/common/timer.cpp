#include "timer.h"
#include <GLFW/glfw3.h>

namespace rade
{
    timer::timer()
    {
        Start();
    }

    void timer::Start()
    {
        m_start_time = static_cast<float>(glfwGetTime());
    }

    // returns time elapses since start() or constructor called
    float timer::ElapsedTime() const
    {
        auto now = static_cast<float>(glfwGetTime());
        return now - m_start_time;
    }
}
