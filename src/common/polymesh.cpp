#include "polymesh.h"
#include "osutils.h"
#include "material.h"
#include "materialmanager.h"
#include "display_gl.h"

namespace rade
{
    void polymesh::AddPoly(const poly3d& poly)
    {
        m_polyList.emplace_back(poly);
    }

    void polymesh::AddPolyList(std::vector<poly3d>& polyList)
    {
        for (poly3d& poly : polyList)
        {
            AddPoly(poly);
        }
    }

    void polymesh::SetShaderKey(const std::string& shaderKey)
    {
        for (poly3d& poly : m_polyList)
        {
            poly.SetShaderKey(shaderKey);
        }
    }

    bool polymesh::LoadMaterials(CMaterialManager& materialMgr, const std::string& extraPath /* = "" */)
    {
        using namespace RMaterials;

        Assert(!m_polyList.empty(), "LoadMaterial called on mesh with 0 polygons\n");
        bool success = true;
        // generate
        for (poly3d& poly : m_polyList)
        {
            materialMgr.LoadFromKey(poly.GetMaterialKey());
        }
        return success;
    }

    void polymesh::LoadLightmaps(CMaterialManager& materialMgr, std::vector<CLightmapImg*>& lightmaps)
    {
        Assert(!m_polyList.empty(), "LoadLightmaps called, but no polygons loaded\n");

        ClearLightmaps();

        // generate
        unsigned int counter = 0;
        for (auto& lm : lightmaps)
        {
            char strKey[32];
            sprintf(strKey, "mesh_lm_%d", counter);

            lightmapInfo_t lmInfo{};

            uint32_t texID = 0;

            bool genMipMaps = true;

            // CLAMP_TO_EDGE and no mip maps are required for Lightmaps (or they will bleed at edges)
            RMaterials::ETextureFilterMode filterMode = RMaterials::TEXTURE_FILTER_MIPMAPLINEAR; //RMaterials::TEXTURE_FILTER_LINEAR;
            RMaterials::ETextureClampMode clampMode = RMaterials::TEXTURE_REPEAT_CLAMP_TO_EDGE;

            bool loaded = materialMgr.LoadRAWTextureData(
                    lm->m_data,
                    lm->m_width,
                    lm->m_height,
                    4, // TODO: this should be reduced to 3
                    genMipMaps,
                    filterMode,
                    clampMode,
                    &texID);
            if (!loaded)
            {
                texID = 0;
                Log("failed to load RAW texture data in LoadLightmaps()\n");
            }

            lmInfo.width = lm->m_width;
            lmInfo.height = lm->m_height;
            lmInfo.channels = 4;
            lmInfo.texID = texID;

            m_lightmaps.push_back(lmInfo);
            counter++;
        }

        // assign the new texture ids back onto the polys
        if (!m_lightmaps.empty())
        {
            m_hasLightmaps = true;
            for (poly3d& poly : m_polyList)
            {
                uint32_t lightDataIndex = poly.GetLightmapDataIndex();
                uint32_t textureID = m_lightmaps.at(lightDataIndex).texID;

                poly.SetLightTexID(textureID);
            }
        }
    }

    void polymesh::Reset()
    {
        // TODO: and delete material from materialmgr too?
//        for (auto& lmap : m_lightmaps)
//        {
//            if (lmap.texID)
//                m_display->GetMaterialMgr().DeleteTextureID(lmap.texID);
//        }
        m_lightmaps.clear();

        m_polyList.clear();
        m_hasLightmaps = false;
    }

    void polymesh::ClearLightmaps()
    {
        m_lightmaps.clear();
        for (poly3d& poly : m_polyList)
        {
            poly.SetLightTexID(0);
        }
    }
};
