#pragma once

#include <map>
#include <meshfile.h>
#include "polygon3d.h"

class CMaterialManager;

struct lightmapInfo_t
{
    uint32_t texID;
    unsigned int width;
    unsigned int height;
    unsigned int channels;
};

class CPolyMesh
{
public:
    void AddPoly(const CPoly3D& poly);

    void AddPolyList(std::vector<CPoly3D>& polyList);

    bool LoadMaterials(CMaterialManager& materialMgr, const std::string& extraPath = "");

    void LoadLightmaps(CMaterialManager& materialMgr, std::vector<CLogicalLightmap>& lightmaps);

    bool HasLightmaps()
    {
        return m_hasLightmaps;
    }

    std::vector<CPoly3D>& GetPolyListRef()
    {
        return m_polyList;
    }

    void Clear();

private:
    std::vector<CPoly3D> m_polyList;
    std::vector<lightmapInfo_t> m_lightmaps;
    bool m_hasLightmaps = false;
};
