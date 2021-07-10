#pragma once

#include <string>

namespace rade
{
    class Image
    {
    public:

        // Represents the number of channels per pixel, and the order of the channels.
        // Each channel is one byte (unsigned char).

        enum Format
        {
            Format_Grayscale = 1, // one channel: grayscale
            Format_GrayscaleAlpha = 2, // two channels: grayscale and alpha
            Format_RGB = 3, // three channels: red, green, blue
            Format_RGBA = 4 // four channels: red, green, blue, alpha
        };

        // Creates a new image with the specified GetWidth, GetHeight and GetFormat.
        // Width and GetHeight are in pixels. Image will contain random garbage if
        // pixels = NULL.
        Image(unsigned int width,
                unsigned int height,
                Format format,
                const unsigned char* pixels);

        Image();

        ~Image();

        bool LoadFile(const std::string& filePath, bool usePlatformAsset = false);

        bool LoadMem(void* buff, size_t size);

        bool SavePNG(const std::string& filePath);

        // GetWidth in pixels
        unsigned GetWidth() const;

        // GetHeight in pixels
        unsigned GetHeight() const;

        // the pixel GetFormat of the bitmap
        Format GetFormat() const;

        // Pointer to the raw pixel data of the bitmap.
        // Each channel is 1 byte. The number and meaning of channels per pixel is specified
        // by the `Format` of the image. The pointer points to all the columns of
        // the top row of the image, followed by each remaining row down to the bottom.
        // i.e. c0r0, c1r0, c2r0, ..., c0r1, c1r1, c2r1, etc
        unsigned char* GetPixelBuffer() const;

        // Returns a pointer to the start of the pixel at the given coordinates.
        // The size of the pixel depends on the `Format` of the image.
        unsigned char* GetPixel(unsigned int column, unsigned int row) const;

        // Sets the raw pixel data at the given coordinates.
        // The size of the pixel depends on the `Format` of the bitmap.
        void SetPixel(unsigned int column, unsigned int row, const unsigned char* pixel);

        // Reverses the row order of the pixels, so the bitmap will be upside down.
        void FlipVertically();

        // Rotates the image 90 degrees counter clockwise.
        void Rotate90CounterClockwise();

        void GetRGBAArray(int col, int row, unsigned char* rgba);

        void Blur();

    private:
        Format m_format;
        unsigned m_width;
        unsigned m_height;
        unsigned char* m_pixels;

        void Set(unsigned width,
                unsigned height,
                Format format,
                const unsigned char* pixels);

        void SetFromPtr(unsigned width,
                unsigned height,
                Format format,
                unsigned char* pixels);

        unsigned int
        GetPixelOffset(unsigned col, unsigned row, unsigned width, Image::Format format) const;
    };
};
