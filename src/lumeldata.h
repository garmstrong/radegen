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

    CPoint3D* m_color;
    CPoint3D* m_pos;
    int m_width;
    int m_height;

    size_t index(int x, int y) const
    {
        return x + m_width * y;
    }

    void SetPosition(int x, int y, const CPoint3D& pos)
    {
        m_pos[index(x, y)] = pos;
    }

    CPoint3D* GetPosition(uint16_t x, uint16_t y)
    {
        return &m_pos[index(x, y)];
    }

    CPoint3D* GetColor(uint16_t x, uint16_t y)
    {
        return &m_color[index(x, y)];
    }

    void SetColor(uint16_t x, uint16_t y, const CPoint3D& color)
    {
        m_color[index(x, y)] = color;
    }

    void Allocate(uint16_t width, uint16_t height)
    {
        m_width = width;
        m_height = height;

        m_color = new CPoint3D[width * height]{};
        m_pos = new CPoint3D[width * height]{};
    }

    void Free()
    {
        delete[] m_color;
        delete[] m_pos;
    }
};
