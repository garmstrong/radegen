#pragma once

namespace rade
{
    class InputSystem
    {
    public:
        bool IsPressed(int keyCode);

        void Press(int keyCode);

        void Release(int keyCode);

    private:
        const int m_cNumKeys = 512;
        bool m_keystates[512]{};
    };
};
