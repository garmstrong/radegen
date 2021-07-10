#pragma once

#include "point3d.h"

class LumelData
{
public:
    LumelData(uint16_t width, uint16_t height)
    {
        Allocate(width, height);
    }

    ~LumelData()
    {
        Free();
    }

    rade::vector3* m_color;
    rade::vector3* m_pos;
    int m_width;
    int m_height;

    size_t index(int x, int y) const
    {
        return x + m_width * y;
    }

    void SetPosition(int x, int y, const rade::vector3& pos)
    {
        m_pos[index(x, y)] = pos;
    }

    rade::vector3* GetPosition(uint16_t x, uint16_t y)
    {
        return &m_pos[index(x, y)];
    }

    rade::vector3* GetColor(uint16_t x, uint16_t y)
    {
        return &m_color[index(x, y)];
    }

    void SetColor(uint16_t x, uint16_t y, const rade::vector3& color)
    {
        m_color[index(x, y)] = color;
    }

    void Allocate(uint16_t width, uint16_t height)
    {
        m_width = width;
        m_height = height;

        m_color = new rade::vector3[width * height]{};
        m_pos = new rade::vector3[width * height]{};
    }

    void Free()
    {
        delete[] m_color;
        delete[] m_pos;
    }
};
