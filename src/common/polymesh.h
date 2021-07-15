#pragma once

#include <map>

#include "polygon3d.h"
#include "lightmapimage.h"

class CMaterialManager;

class CDisplayGL;

class IRenderObj;

namespace rade
{
    class Camera;

    class polymesh
    {
    public:

        struct lightmapInfo_t
        {
            unsigned int texID;
            unsigned int width;
            unsigned int height;
            unsigned int channels;
        };

        //bool RegisterWithDisplay(CDisplayGL& display, Camera* camera);

        void AddPoly(const rade::poly3d& poly);

        void AddPolyList(std::vector<rade::poly3d>& polyList);

        bool LoadMaterials(CMaterialManager& materialMgr, const std::string& extraPath = "");

        void LoadLightmaps(CMaterialManager& materialMgr, std::vector<CLightmapImg*>& lightmaps);

        bool HasLightmaps() const
        {
            return m_hasLightmaps;
        }

        std::vector<rade::poly3d>& GetPolyListRef()
        {
            return m_polyList;
        }

        void Reset();

        void ClearLightmaps();

        std::vector<lightmapInfo_t>& GetLoadedLightmapInfoRef()
        {
            return m_lightmaps;
        }

        void SetShaderKey(const std::string& shaderKey);

        //Camera* m_camera = nullptr;

    private:
        std::vector<rade::poly3d> m_polyList;
        std::vector<lightmapInfo_t> m_lightmaps;
        bool m_hasLightmaps = false;

        //CDisplayGL* m_display = nullptr;

    };

};
