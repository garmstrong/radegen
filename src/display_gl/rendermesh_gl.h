#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "rendertypes.h"
#include "shader_gl.h"

#include <string>

class CDisplayGL;

class CMaterial;

namespace rade
{
    class polymesh;

    class Camera;
};

class CRenderMeshGL
{
public:

    struct vertBuffer_t
    {
        NRenderTypes::Vert* vertBuffer;
        uint16_t numVerts;
        unsigned int glVBOId;
        std::string materialName;
        CMaterial *mat;
        uint16_t copiedSoFar;
    };

    void InitFromPolyMesh(rade::polymesh& renderMesh);

    void AddFace(NRenderTypes::Face& face);

    void RenderAllFaces(const rade::Camera& cam);

    void Reset();

    void LoadMeshTexures(CDisplayGL& displayGl, bool usePlatformAssets = false);

    void SetRenderMode(NRenderTypes::ERenderMode renderMode)
    {
        m_renderMode = renderMode;
    }

    NRenderTypes::ERenderMode GetRenderMode()
    {
        return m_renderMode;
    }

    void PrepareMesh(CDisplayGL& displayGl, bool loadTextures, bool usePlatformAssets = false);

private:

    unsigned int m_vaoId;

    NRenderTypes::ERenderMode m_renderMode = NRenderTypes::ERenderDefault;

    std::vector<NRenderTypes::Face> m_tmpFaces;
    std::map<std::string, vertBuffer_t> m_vertBuffers;

    void OnRenderStart();

    void OnRenderFinish();

    Shader m_meshShader;

    glm::mat4 m_model = glm::mat4(1);

    bool m_hasLightmaps = false;
};
