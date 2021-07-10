#pragma once

#include "point3d.h"
#include <cstdint>

class CLightmapImg
{
public:
    CLightmapImg()
    {
        m_width = 0;
        m_height = 0;
        m_data = nullptr;
    }

    CLightmapImg(int width, int height)
    {
        m_width = width;
        m_height = height;
        Allocate(width, height);
    }

    ~CLightmapImg()
    {
        //Free();
    }

    uint16_t m_width = 0;
    uint16_t m_height = 0;
    unsigned char* m_data = nullptr;
    uint16_t m_numCombines = 1;

    size_t index(int x, int y) const
    {
        return x * 4 + m_width * y * 4;
    }

    unsigned char* GetPixel(int x, int y)
    {
        return &m_data[index(x, y)];
    }

    void SetPixel(int x, int y, unsigned char* rgba)
    {
        size_t offset = index(x, y);
        m_data[offset + 0] = rgba[0];
        m_data[offset + 1] = rgba[1];
        m_data[offset + 2] = rgba[2];
        m_data[offset + 3] = 255;
    }

    void SetPixel(int x, int y, const rade::vector3& p)
    {
        size_t offset = index(x, y);
        m_data[offset + 0] = static_cast<unsigned char>(p.x);
        m_data[offset + 1] = static_cast<unsigned char>(p.y);
        m_data[offset + 2] = static_cast<unsigned char>(p.z);
        m_data[offset + 3] = 255;
    }

    void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        size_t offset = index(x, y);
        m_data[offset + 0] = r;
        m_data[offset + 1] = g;
        m_data[offset + 2] = b;
        m_data[offset + 3] = 255;
    }

    void Allocate(uint16_t width, uint16_t height)
    {
        m_width = width;
        m_height = height;
        if (m_data)
            delete[] m_data;

        m_data = new unsigned char[width * height * 4]{};
    }

    void Free()
    {
        if (m_data)
        {
            delete[] m_data;
            m_data = nullptr;
        }
    }

    void Combine(CLightmapImg& lmOther)
    {
        m_numCombines++;
        if (m_width != lmOther.m_width || m_height != lmOther.m_height)
            return;

        for (uint16_t iX = 0; iX < lmOther.m_width; iX++)
        {
            for (uint16_t iY = 0; iY < lmOther.m_height; iY++)
            {
                unsigned char* p = GetPixel(iX, iY);
                unsigned char* op = lmOther.GetPixel(iX, iY);

                unsigned int newRGBA[4];
                newRGBA[0] = p[0] + op[0];
                newRGBA[1] = p[1] + op[1];
                newRGBA[2] = p[2] + op[2];
                newRGBA[3] = 254;

                newRGBA[0] /= m_numCombines;
                newRGBA[1] /= m_numCombines;
                newRGBA[2] /= m_numCombines;

                unsigned char cRGBA[4];
                cRGBA[0] = static_cast<unsigned char>(newRGBA[0]);
                cRGBA[1] = static_cast<unsigned char>(newRGBA[1]);
                cRGBA[2] = static_cast<unsigned char>(newRGBA[2]);
                cRGBA[3] = 254;

                SetPixel(iX, iY, cRGBA);

            }
        }
    }
};
