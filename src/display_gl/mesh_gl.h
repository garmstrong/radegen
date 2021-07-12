#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "rendertypes.h"
#include "shader_gl.h"
#include "irenderobj.h"

#include <string>

class CDisplayGL;

class CMaterial;

class CMaterialManager;

namespace rade
{
    class polymesh;

    class Camera;
};

class CMeshGL : public IRenderObj
{
public:
    CMeshGL()
    = default;

    ~CMeshGL() override
    = default;

    CMeshGL(rade::polymesh& polymesh, CMaterialManager& matMgr);

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

    void InitFromPolyMesh(rade::polymesh& renderMesh);

    void AddFace(NRenderTypes::Tri& face);

    void RenderAllFaces(CDisplayGL *display);

    void Reset();

    void LoadMeshTexures(CMaterialManager& materialMgr, bool usePlatformAssets = false);

    void SetRenderMode(NRenderTypes::ERenderMode renderMode)
    {
        m_renderMode = renderMode;
    }

    NRenderTypes::ERenderMode GetRenderMode()
    {
        return m_renderMode;
    }

    void PrepareMesh();

    void GetBoundingBox() override{};

private:

    NRenderTypes::ERenderMode m_renderMode = NRenderTypes::ERenderDefault;

    std::vector<NRenderTypes::Tri> m_tmpFaces;
    std::map<std::string, vertBuffer_t> m_vertBuffers;

    glm::mat4 m_model = glm::mat4(1);

    bool m_hasLightmaps = false;
};
