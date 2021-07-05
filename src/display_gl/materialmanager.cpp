#include <string>
#include "materialmanager.h"
#include "material.h"
#include "osutils.h"
#include "image.h"
#include "display_gl.h"

CMaterialManager::CMaterialManager(CDisplayGL& display) :
        m_display(display)
{
}

CMaterialManager::~CMaterialManager()
= default;

CMaterial* CMaterialManager::FindMaterial(const std::string& materialKey)
{
    CMaterial* material = nullptr;
    if (m_materials.find(materialKey) != m_materials.end())
    {
        material = &m_materials.find(materialKey)->second;
    }
    return material;
}

CMaterial* CMaterialManager::LoadFromKey(const std::string& key)
{
    using namespace RMaterials;

    // check its not loaded already
    CMaterial* matPtr = FindMaterial(key);
    if (matPtr)
    {
        return matPtr;
    }

    // key is "d4/blocks_1_2"
    // if "d4/blocks_1_2.mat" exists, parse the complex material
    if (OS::FileExists(key + ".mat"))
    {
        // TODO:
        // parse it..
        return nullptr;
    }
    else
    {
        // load a simple diffuse material (from simple filename)
        std::string filename = KeyToFilename(key);
        if (filename.empty() || !OS::FileExists(filename))
        {
            OS::Log("Failed to load simple material from key: %s : file: %s\n",
                    key.c_str(), filename.c_str());
            return nullptr;
        }

        CMaterial newMat;
        CTextureProperties* diffuseProps = newMat.GetTextureProps(TEXTURE_SLOT_DIFFUSE);
        diffuseProps->textureFilePath = filename;
        diffuseProps->clampMode = TEXTURE_REPEAT_REPEAT;
        diffuseProps->filterMode = TEXTURE_FILTER_MIPMAPLINEAR;

        CImage bmp;
        bool loaded = bmp.LoadFile(filename);
        if (!loaded)
        {
            OS::Log("Failed to load texture file %s for material key %s\n", filename.c_str(), key.c_str());
            return nullptr;
        }

        OS::Assert(bmp.GetFormat() == CImage::Format_RGB || bmp.GetFormat() == CImage::Format_RGBA,
                "invalid image format\n");

        bool genMipMaps = true;

        uint32_t newID;
        bool uploaded = m_display.LoadRAWTextureData(bmp.GetPixelBuffer(),
                static_cast<int>(bmp.GetWidth()),
                static_cast<int>(bmp.GetHeight()),
                bmp.GetFormat() == CImage::Format_RGB ? 3 : 4,
                genMipMaps,
                diffuseProps->filterMode,
                diffuseProps->clampMode,
                &newID);

        OS::Assert(uploaded, "Failed to upload texture %s!\n", filename.c_str());
        newMat.GetTextureProps(TEXTURE_SLOT_DIFFUSE)->loadedTextureID = newID;

        // insert to list
        m_materials[key] = newMat;
        CMaterial* newMaterial = &m_materials[key];
        return newMaterial;
    }
}

std::string CMaterialManager::KeyToFilename(const std::string& key)
{
    std::string filenameNoExt = OS::TexturePath(key);
    std::vector<std::string> ext = { ".tga", ".png", ".jpg", ".jpeg" };
    bool fileFound = false;
    std::string fullPathFileName;
    for (auto& i : ext)
    {
        if (OS::FileExists(filenameNoExt + i))
        {
            fullPathFileName = filenameNoExt + i;
            fileFound = true;
            break;
        }
    }
    if (!fileFound)
    {
        OS::Log("Failed to find a matching file for material key %s\n", key.c_str());
        return "";
    }
    return fullPathFileName;
}

bool CMaterialManager::LoadRAWTextureData(
        const unsigned char* data,
        const int width,
        const int height,
        const int channels,
        const bool genMipMaps,
        const RMaterials::ETextureFilterMode filterMode,
        const RMaterials::ETextureClampMode clampMode,
        uint32_t* id)
{
    uint32_t newID;
    bool loaded = m_display.LoadRAWTextureData(
            data,
            width, height,
            channels,
            genMipMaps,
            filterMode,
            clampMode,
            &newID);

    if (loaded)
    {
        *id = newID;
    }
    else
    {
        *id = 0;
    }
    return loaded;
}
