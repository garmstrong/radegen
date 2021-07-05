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

CImage::CImage() :
        m_format(Format_RGB),
        m_width(0),
        m_height(0),
        m_pixels(nullptr)
{
}

CImage::CImage(unsigned int width, unsigned int height, Format format, const unsigned char* pixels) :
        m_format(Format_RGB),
        m_width(0),
        m_height(0),
        m_pixels(nullptr)
{
    Set(width, height, format, pixels);
}

CImage::~CImage()
{
    if (m_pixels)
    {
        free(m_pixels);
    }
}

bool CImage::LoadFile(const std::string& filePath, bool usePlatformAsset /*= false*/)
{
    CTimer time;

    long size = 0;
    char* buff = nullptr;
    if (usePlatformAsset)
    {
        buff = OS::ReadPlatformAssetFile(filePath.c_str(), &size);
    }
    else
    {
        buff = OS::ReadFile(filePath.c_str(), &size);
    }
    if (size == 0 || buff == nullptr)
    {
        OS::Log("File %s could not be found\n", filePath.c_str());
        return false;
    }

    int width, height, channels;

    //m_mutex.lock();
    unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buff), size, &width, &height,
            &channels, 0);
    if (!pixels)
    {
        return false;
    }

    // copy to local data and delete buffer
    SetFromPtr(width, height, (Format)channels, pixels);

    return true;
}

bool CImage::LoadMem(void* buff, size_t size)
{
    int width, height, channels;
    unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buff), size, &width, &height,
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
        return true;
    }
}

bool CImage::SavePNG(const std::string& filePath)
{
	return stbi_write_png(filePath.c_str(), m_width, m_height, m_format, m_pixels, m_width*4);
}

unsigned int CImage::GetWidth() const
{
    return m_width;
}

unsigned int CImage::GetHeight() const
{
    return m_height;
}

CImage::Format CImage::GetFormat() const
{
    return m_format;
}

unsigned char* CImage::GetPixelBuffer() const
{
    return m_pixels;
}

unsigned int CImage::GetPixelOffset(unsigned col, unsigned row, unsigned width, CImage::Format format) const
{
    return (row * width + col) * format;
}

unsigned char* CImage::GetPixel(unsigned int column, unsigned int row) const
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

void CImage::SetPixel(unsigned int column, unsigned int row, const unsigned char* pixel)
{
    unsigned char* myPixel = GetPixel(column, row);
    memcpy(myPixel, pixel, m_format);
}

void CImage::FlipVertically()
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

void CImage::Rotate90CounterClockwise()
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

void CImage::GetRGBAArray(int col, int row, unsigned char* rgba)
{
    unsigned srcOffset = GetPixelOffset(col, row, m_width, m_format);
    for (int i = 0; i < m_format; i++)
        rgba[i] = *(m_pixels + srcOffset + i);
}

void CImage::Blur()
{
    struct blurKernel_t
    {
        int xOffset;
        int yOffset;
        float weight;
    };

    std::vector<blurKernel_t> blurKernel = {
            { -1, -1, 1 / 4 },  // TL
            { 0,  -1, 1 / 4 },  // TM
            { 1,  1,  1 / 4 },  // TR
            { -1, 0,  1 / 4 },  // CL
            { 0,  0,  1 },  // C
            { 1,  0,  1 / 4 },  // CR
            { -1, 1,  1 / 4 },  // BL
            { 0,  1,  1 / 4 },  // BM
            { 1,  1,  1 / 4 }};// BR

    auto newPixels = (unsigned char*)malloc(m_format * m_width * m_height);

    for (unsigned iX = 0; iX < m_width; ++iX)
    {
        for (unsigned iY = 0; iY < m_height; ++iY)
        {
            float totalWeight = 0;
            unsigned long avg_rgba[4] = { 0, 0, 0, 0 };
            for (size_t i = 0; i < blurKernel.size(); i++)
            {
                unsigned char rgba[4];
                int xoff = iX + blurKernel.at(i).xOffset;
                if (xoff < 0) xoff = 1;
                if (xoff >= m_width) xoff = m_width - 2;
                int yoff = iY + blurKernel.at(i).yOffset;
                if (yoff < 0) yoff = 1;
                if (yoff >= m_height) yoff = m_height - 2;
                GetRGBAArray(xoff, yoff, rgba);

                float weight = blurKernel.at(i).weight;
                avg_rgba[0] += rgba[0];// * weight;
                avg_rgba[1] += rgba[1];// * weight;
                avg_rgba[2] += rgba[2];// * weight;
                avg_rgba[3] += rgba[3];// * weight;
                //totalWeight += weight;
            }
            totalWeight = blurKernel.size();
            avg_rgba[0] /= totalWeight;
            avg_rgba[1] /= totalWeight;
            avg_rgba[2] /= totalWeight;
            avg_rgba[3] /= totalWeight;

            unsigned char final_rgba[4];
            final_rgba[0] = avg_rgba[0];
            final_rgba[1] = avg_rgba[1];
            final_rgba[2] = avg_rgba[2];
            final_rgba[3] = avg_rgba[3];

            unsigned int destOffset = GetPixelOffset(iX, iY, m_width, m_format);
            memcpy(newPixels + destOffset, final_rgba, 4); //copy one pixel
        }
    }

    free(m_pixels);
    m_pixels = newPixels;
}

void CImage::Set(unsigned width,
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

void CImage::SetFromPtr(unsigned width,
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
