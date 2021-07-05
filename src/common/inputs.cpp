#include "inputs.h"

CInputSystem::CInputSystem()
{
    //m_keystates = new bool[m_cNumKeys];
}

CInputSystem::~CInputSystem()
{
    //delete []m_keystates;
}

bool CInputSystem::IsPressed(const int keyCode)
{
    if (keyCode < 0 || keyCode >= m_cNumKeys)
        return false;

    return m_keystates[keyCode];
}

void CInputSystem::Press(const int keyCode)
{
    if (keyCode < 0 || keyCode >= m_cNumKeys)
        return;

    m_keystates[keyCode] = true;
}

void CInputSystem::Release(const int keyCode)
{
    if (keyCode < 0 || keyCode >= m_cNumKeys)
        return;

    m_keystates[keyCode] = false;
}

//void CInputSystem::MouseEvent(const int keyCode, int x, int y, bool downEvent)
//{
//    if(keyCode < 0 || keyCode >= m_cNumKeys)
//        return;
//
//    SMousePoint mouseData;
//    mouseData.x = (float)x;
//    mouseData.y = (float)y;
//    mouseData.downEvent = downEvent;
//    mouseData.keycode = keyCode;
//
//    void *p = &mouseData;
//
//    // tell the Observers what happened
//    for (auto & inputOb : inputObs)
//    {
//        inputOb->InputEvent( keyCode, downEvent, p ) ;
//    }
//}
//
//// Debounce a key, returns the previous key state
//bool CInputSystem::Debounce(const int keyCode)
//{
//    if(keyCode < 0 || keyCode >= m_cNumKeys)
//        return false;
//
//    bool oldState = m_eKeyDown[keyCode];
//    m_eKeyDown[keyCode] = false;
//    return oldState;
//}
//
//// Debounce all the keys.
//void CInputSystem::DebounceAll()
//{
//    for (int i = 0 ; i < m_cNumKeys ; ++i)
//    {
//    	m_eKeyDown[i] = false; //set it up by default
//    }
//}

//// Clear all input - debounce all keys and clear queue
//void CInputSystem::Clear()
//{
//    DebounceAll();
//}
