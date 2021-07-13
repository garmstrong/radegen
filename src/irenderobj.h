#pragma once
#include <string>
#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "point3d.h"
#include "transform.h"

namespace rade
{
    class Camera;
}

class IRenderObj
{
public:
    IRenderObj()
    = default;

    virtual ~IRenderObj()
    = default;

    virtual rade::transform& GetTransform()
    {
        return m_transform;
    }

    virtual void SetEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    virtual bool GetEnabled() const
    {
        return m_enabled;
    }

    virtual void SetCamera(rade::Camera* camera)
    {
        m_camera = camera;
    }

    // used for text mesh objects, and debugging/editor
    virtual void SetText(const char* pszFormat, ...)
    {
        static char szBuff[512];
        va_list vArgs;
        va_start(vArgs, pszFormat);
        vsprintf(szBuff, pszFormat, vArgs);
        va_end(vArgs);
        m_text = szBuff;
    }

    virtual void SetText(const std::string& text)
    {
        m_text = text;
    }

    virtual std::string GetText()
    {
        return m_text;
    }

    virtual void GetBoundingBox() = 0;

protected:
    bool m_enabled = false;
    rade::Camera* m_camera = nullptr;
    rade::transform m_transform;

private:
    std::string m_text;
};
