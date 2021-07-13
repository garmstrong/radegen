#include "image.h"
#include <cstdlib>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"
#include "stb_image.h"
#include "osutils.h"
#include "timer.h"

namespace rade
{
    Image::Image() :
            m_format(Format_RGB),
            m_width(0),
            m_height(0),
            m_pixels(nullptr)
    {
    }

    Image::Image(unsigned int width, unsigned int height, Format format, const unsigned char* pixels) :
            m_format(Format_RGB),
            m_width(0),
            m_height(0),
            m_pixels(nullptr)
    {
        Set(width, height, format, pixels);
    }

    Image::~Image()
    {
        if (m_pixels)
        {
            free(m_pixels);
        }
    }

    bool Image::LoadFile(const std::string& filePath, bool usePlatformAsset /*= false*/)
    {
        timer time;

        long size = 0;
        char* buff = nullptr;
        if (usePlatformAsset)
        {
            buff = rade::ReadPlatformAssetFile(filePath.c_str(), &size);
        }
        else
        {
            buff = rade::ReadFile(filePath.c_str(), &size);
        }
        if (size == 0 || buff == nullptr)
        {
            rade::Log("File %s could not be found\n", filePath.c_str());
            return false;
        }

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buff), size, &width, &height,
                &channels, 0);
        if (!pixels)
        {
            return false;
        }
        free(buff);

        // copy to local data and delete buffer
        SetFromPtr(width, height, (Format)channels, pixels);
        return true;
    }

    bool Image::LoadMem(void* buff, size_t size)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buff), static_cast<int>(size),
                &width, &height,
                &channels, 0);
        if (!pixels)
        {
            return false;
        }
        else
        {
            // copy to local data and delete buffer
            Set(width, height, (Format)channels, pixels);
            stbi_image_free(pixels);
            FlipVertically();
            return true;
        }
    }

    bool Image::SavePNG(const std::string& filePath)
    {
        return stbi_write_png(filePath.c_str(), m_width, m_height, m_format, m_pixels, m_width * 4);
    }

    unsigned int Image::GetWidth() const
    {
        return m_width;
    }

    unsigned int Image::GetHeight() const
    {
        return m_height;
    }

    Image::Format Image::GetFormat() const
    {
        return m_format;
    }

    unsigned char* Image::GetPixelBuffer() const
    {
        return m_pixels;
    }

    unsigned int Image::GetPixelOffset(unsigned col, unsigned row, unsigned width, Image::Format format) const
    {
        return (row * width + col) * format;
    }

    unsigned char* Image::GetPixel(unsigned int column, unsigned int row) const
    {
        if (row >= m_height)
        {
            row = m_height - 1;
        }

        if (column >= m_width)
        {
            column = m_width - 1;
        }

        if (row < 0)
        {
            row = 0;
        }

        if (column < 0)
        {
            column = 0;
        }

        return m_pixels + GetPixelOffset(column, row, m_width, m_format);
    }

    void Image::SetPixel(unsigned int column, unsigned int row, const unsigned char* pixel)
    {
        unsigned char* myPixel = GetPixel(column, row);
        memcpy(myPixel, pixel, m_format);
    }

    void Image::FlipVertically()
    {
        unsigned long rowSize = m_format * m_width;
        auto rowBuffer = new unsigned char[rowSize];
        unsigned halfRows = m_height / 2;

        for (unsigned rowIdx = 0; rowIdx < halfRows; ++rowIdx)
        {
            unsigned char* row = m_pixels + GetPixelOffset(0, rowIdx, m_width, m_format);
            unsigned char* oppositeRow = m_pixels + GetPixelOffset(0, m_height - rowIdx - 1, m_width, m_format);
            memcpy(rowBuffer, row, rowSize);
            memcpy(row, oppositeRow, rowSize);
            memcpy(oppositeRow, rowBuffer, rowSize);
        }
        delete[] rowBuffer;
    }

    void Image::Rotate90CounterClockwise()
    {
        auto newPixels = (unsigned char*)malloc(m_format * m_width * m_height);

        for (unsigned row = 0; row < m_height; ++row)
        {
            for (unsigned col = 0; col < m_width; ++col)
            {
                unsigned srcOffset = GetPixelOffset(col, row, m_width, m_format);
                unsigned destOffset = GetPixelOffset(row, m_width - col - 1, m_height, m_format);
                memcpy(newPixels + destOffset, m_pixels + srcOffset, m_format); //copy one pixel
            }
        }

        free(m_pixels);
        m_pixels = newPixels;

        unsigned swapTmp = m_height;
        m_height = m_width;
        m_width = swapTmp;
    }

    void Image::GetRGBAArray(int col, int row, unsigned char* rgba)
    {
        if(col>=(int)m_width) col = m_width-1;
        if(row>=(int)m_height) row = m_height-1;
        if(col<0) col = 0;
        if(row<0) row = 0;

        unsigned srcOffset = GetPixelOffset(col, row, m_width, m_format);
        for (int i = 0; i < m_format; i++)
            rgba[i] = *(m_pixels + srcOffset + i);
    }

    void Image::Blur()
    {
        struct blurKernel_t
        {
            int xOffset;
            int yOffset;
            float weight;
        };

        std::vector<blurKernel_t> blurKernel = {
                { -1, -1, 0.2f },  // TL
                { 0,  -1, 0.2f },  // TM
                { 1,  1,  0.2f },  // TR
                { -1, 0,  0.2f },  // CL
                { 0,  0,  0.3f },  // C
                { 1,  0,  0.2f },  // CR
                { -1, 1,  0.2f },  // BL
                { 0,  1,  0.2f },  // BM
                { 1,  1,  0.2f }   // BR
        };

        float totalWeight = 0.0f;
        for (auto & kern : blurKernel)
        {
            totalWeight += kern.weight;
        }

        auto newPixels = (unsigned char*)malloc(m_format * m_width * m_height);

        for (unsigned iX = 0; iX < m_width; ++iX)
        {
            for (unsigned iY = 0; iY < m_height; ++iY)
            {
                float avg_rgba[4] = { 0, 0, 0, 0 }; // allow over "saturation" and divide later
                for (auto & kern : blurKernel)
                {
                    unsigned char rgba[4];
                    GetRGBAArray(iX + kern.xOffset, iY + kern.yOffset, rgba);

                    float multiplierAvg = kern.weight / totalWeight;
                    avg_rgba[0] += (float)rgba[0] * multiplierAvg;
                    avg_rgba[1] += (float)rgba[1] * multiplierAvg;
                    avg_rgba[2] += (float)rgba[2] * multiplierAvg;
                }

                unsigned char final_rgba[4];
                final_rgba[0] = static_cast<unsigned char>(avg_rgba[0]);
                final_rgba[1] = static_cast<unsigned char>(avg_rgba[1]);
                final_rgba[2] = static_cast<unsigned char>(avg_rgba[2]);
                final_rgba[3] = 255; //avg_rgba[3];

                unsigned int destOffset = GetPixelOffset(iX, iY, m_width, m_format);
                memcpy(newPixels + destOffset, final_rgba, 4); //copy one pixel
            }
        }

        free(m_pixels);
        m_pixels = newPixels;
    }

    void Image::Set(unsigned width,
            unsigned height,
            Format format,
            const unsigned char* pixels)
    {

        if (width == 0) return;
        if (height == 0) return;
        if (format <= 0 || format > 4) return;

        m_width = width;
        m_height = height;
        m_format = format;

        size_t newSize = m_width * m_height * m_format;
        if (m_pixels)
        {
            m_pixels = (unsigned char*)realloc(m_pixels, newSize);
        }
        else
        {
            m_pixels = (unsigned char*)malloc(newSize);
        }

        if (pixels)
        {
            memcpy(m_pixels, pixels, newSize);
        }
    }

    void Image::SetFromPtr(unsigned width,
            unsigned height,
            Format format,
            unsigned char* pixels)
    {
        if (width == 0) return;
        if (height == 0) return;
        if (format <= 0 || format > 4) return;

        m_width = width;
        m_height = height;
        m_format = format;
        m_pixels = pixels;
    }
}
