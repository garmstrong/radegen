#include "timer.h"
#include <GLFW/glfw3.h>

namespace rade
{
    timer::timer()
    {
        Start();
    }

    timer::~timer()
    {
        Start();
    }

    void timer::Start()
    {
        m_start_time = glfwGetTime();
    }

    // returns time elapses since start() or constructor called
    double timer::ElapsedTime()
    {
        double now = glfwGetTime();
        return now - m_start_time;
    }
}
