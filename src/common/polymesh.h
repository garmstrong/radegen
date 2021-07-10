#pragma once

#include <map>

#include "polygon3d.h"
#include "lightmapimage.h"

class CMaterialManager;

class CDisplayGL;

namespace rade
{
    class Camera;

    class CPolyMesh
    {
    public:

        struct lightmapInfo_t
        {
            unsigned int texID;
            unsigned int width;
            unsigned int height;
            unsigned int channels;
        };

        bool Init(CDisplayGL& display, Camera* camera);

        void AddPoly(const rade::CPoly3D& poly);

        void AddPolyList(std::vector<rade::CPoly3D>& polyList);

        bool LoadMaterials(CMaterialManager& materialMgr, const std::string& extraPath = "");

        void LoadLightmaps(CMaterialManager& materialMgr, std::vector<CLightmapImg>& lightmaps);

        bool HasLightmaps() const
        {
            return m_hasLightmaps;
        }

        std::vector<rade::CPoly3D>& GetPolyListRef()
        {
            return m_polyList;
        }

        void Reset();

        void ClearLightmaps();

        std::vector<lightmapInfo_t>& GetLoadedLightmapInfoRef()
        {
            return m_lightmaps;
        }

        Camera* m_camera = nullptr;

    private:
        std::vector<rade::CPoly3D> m_polyList;
        std::vector<lightmapInfo_t> m_lightmaps;
        bool m_hasLightmaps = false;

        uint32_t m_meshID = 0;
        CDisplayGL* m_display = nullptr;

    };

};
