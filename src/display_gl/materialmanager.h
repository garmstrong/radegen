#pragma once

#include <string>
#include <map>
#include "material.h"

class CDisplayGL;

class CMaterialManager
{
public:

    explicit CMaterialManager(CDisplayGL& display);

    ~CMaterialManager();

    CMaterial* FindMaterial(const std::string& materialKey);

    CMaterial* LoadFromKey(const std::string& key);

    bool LoadRAWTextureData(
            const unsigned char* data,
            int width,
            int height,
            int channels,
            bool genMipMaps,
            RMaterials::ETextureFilterMode filterMode,
            RMaterials::ETextureClampMode clampMode,
            uint32_t* id);

private:

    std::string KeyToFilename(const std::string& key);

    std::map<std::string, CMaterial> m_materials;
    CDisplayGL& m_display;
};
