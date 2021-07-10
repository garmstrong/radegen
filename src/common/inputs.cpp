#include "inputs.h"

namespace rade
{
    bool InputSystem::IsPressed(const int keyCode)
    {
        if (keyCode < 0 || keyCode >= m_cNumKeys)
            return false;

        return m_keystates[keyCode];
    }

    void InputSystem::Press(const int keyCode)
    {
        if (keyCode < 0 || keyCode >= m_cNumKeys)
            return;

        m_keystates[keyCode] = true;
    }

    void InputSystem::Release(const int keyCode)
    {
        if (keyCode < 0 || keyCode >= m_cNumKeys)
            return;

        m_keystates[keyCode] = false;
    }
};
