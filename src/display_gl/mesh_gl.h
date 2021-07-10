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
    class CPolyMesh;

    class Camera;
};

class CMeshGL
{
public:

    struct vertBuffer_t
    {
        NRenderTypes::Vert* vertBuffer;
        uint16_t numVerts;
        unsigned int glVAOId;
        unsigned int glVBOId;
        std::string materialName;
        std::string shaderName;
        CMaterial* mat;
        uint16_t copiedSoFar;
        unsigned int lightmapID;
    };

    void InitFromPolyMesh(rade::CPolyMesh& renderMesh);

    void AddFace(NRenderTypes::Tri& face);

    void RenderAllFaces(CDisplayGL *display);

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

    void PrepareMesh(CDisplayGL& displayGl);

private:

    CDisplayGL *m_display;
    rade::Camera* m_camera;

    NRenderTypes::ERenderMode m_renderMode = NRenderTypes::ERenderDefault;

    std::vector<NRenderTypes::Tri> m_tmpFaces;
    std::map<std::string, vertBuffer_t> m_vertBuffers;


    //Shader m_meshShader;

    glm::mat4 m_model = glm::mat4(1);

    bool m_hasLightmaps = false;
};
