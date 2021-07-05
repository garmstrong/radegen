#pragma once

#include <vector>

class CInputSystem
{
private:
    const int m_cNumKeys = 512;

    bool m_keystates[512] {};
public:

    CInputSystem();
    ~CInputSystem();

    bool IsPressed(int keyCode);

    void Press(int keyCode);

    void Release(int keyCode);
};
