#pragma once

#include "point3d.h"

class IRenderObj;

namespace rade
{
    class Light
    {
    public:
        std::string name;
        rade::vector3 pos;
        rade::vector3 orientation;
        float radius = 128.0f;
        float brightness = 100.0f;
        float color[3] = { 1.0f, 1.0f, 1.0f };
        IRenderObj *renderObj = nullptr;
    };
};
