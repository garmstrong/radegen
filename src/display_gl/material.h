#pragma once

#include <cstdint>
#include <string>
//#include "image.h"
//#include "display_gl.h"
//#include "osutils.h"

namespace RMaterials
{
    enum ETextureSlots
    {
        TEXTURE_SLOT_DIFFUSE = 0,
        TEXTURE_SLOT_NORMAL,
        TEXTURE_SLOT_SPECULAR,
        TEXTURE_SLOT_END
    };

    enum ETextureClampMode
    {
        TEXTURE_REPEAT_REPEAT = 0,
        TEXTURE_REPEAT_CLAMP_TO_EDGE,
        TEXTURE_REPEAT_CLAMP_TO_BORDER
    };

    enum ETextureFilterMode
    {
        TEXTURE_FILTER_LINEAR = 0,
        TEXTURE_FILTER_MIPMAPLINEAR
    };

    class CTextureProperties
    {
    public:
        ETextureClampMode clampMode = TEXTURE_REPEAT_REPEAT;
        ETextureFilterMode filterMode = TEXTURE_FILTER_MIPMAPLINEAR;
        int numColourChannels = 0;
        std::string textureFilePath;
        uint32_t loadedTextureID = 0;
    };

    class CShaderInfo
    {
    public:
        std::string vertShader;
        std::string fragShader;
        std::string geomShader;
    };
};


class CMaterial
{
public:
    //uint32_t loadedMaterialID = 0;

    RMaterials::CShaderInfo GetShaderInfo() const
    {
        return m_shaderInfo;
    }

    RMaterials::CTextureProperties* GetTextureProps(enum RMaterials::ETextureSlots slot)
    {
        return &m_textures[slot];
    }

private:
    RMaterials::CShaderInfo m_shaderInfo;
    RMaterials::CTextureProperties m_textures[RMaterials::TEXTURE_SLOT_END];

};
