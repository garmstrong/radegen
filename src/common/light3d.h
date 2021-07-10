#pragma once

#include "point3d.h"

namespace rade
{
    class CTextMesh;

    class Light
    {
    public:
        std::string name;
        rade::vector3 pos;
        rade::vector3 orientation;
        float radius = 128.0f;
        float brightness = 100.0f;
        float color[3] = { 1.0f, 1.0f, 1.0f };
        rade::CTextMesh* label = nullptr;
    };
};
