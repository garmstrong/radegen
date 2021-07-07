#pragma once

#include "point3d.h"

class CTextMesh;

class CLight
{
public:
    std::string name;
    CPoint3D pos;
    CPoint3D orientation;
    float radius = 128.0f;
    float brightness = 100.0f;
    float color[3] = { 1.0f, 1.0f, 1.0f };
    CTextMesh *label = nullptr;
};

