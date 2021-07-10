#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <glm/detail/type_mat.hpp>

#include "polygon3d.h"
#include "rendertypes.h"
#include "shader_gl.h"

namespace rade
{
    class Camera;

    class CPolyMesh;
};

class CDisplayGL;

class CRenderDebugMeshGL
{
public:
    CRenderDebugMeshGL();

    ~CRenderDebugMeshGL();

    void AddFace(NRenderTypes::Face& face);

    //void RenderAllFaces();
    void RenderDepthOnly(rade::Camera& cam);
    void RenderAllFacesShadowMapped(rade::Camera& cam);
    void RenderDiffuse(rade::Camera& cam);
    void RenderAllFacesW(rade::Camera& cam);

    void InitFromPolyMesh(rade::CPolyMesh& renderMesh);

    void PrepareMesh(CDisplayGL& displayGl, bool loadTextures);

    void SetRenderMode(NRenderTypes::ERenderMode renderMode)
    {
        m_renderMode = renderMode;
    }

    NRenderTypes::ERenderMode GetRenderMode()
    {
        return m_renderMode;
    }

    void LoadMeshTexures(CDisplayGL& displayGl);

    void Reset();

private:
    std::vector<NRenderTypes::Face> m_faces;
    unsigned int m_vaoId = 0;
    NRenderTypes::ERenderMode m_renderMode = NRenderTypes::ERenderDefault;

    void OnRenderStart();

    void OnRenderFinish();

    CDisplayGL* m_display{};

    Shader m_meshShader;

    glm::mat4 m_model = glm::mat4(1);

    bool m_hasLightmaps = false;

};
