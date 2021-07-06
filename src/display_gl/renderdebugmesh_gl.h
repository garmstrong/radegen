#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <glm/detail/type_mat.hpp>

#include "polygon3d.h"
#include "rendertypes.h"
#include "shader_gl.h"

class Camera;

class CDisplayGL;

class CPolyMesh;

class CRenderDebugMeshGL
{
public:
    CRenderDebugMeshGL();

    ~CRenderDebugMeshGL();

    void AddFace(NRenderMeshGL::Face& face);

    //void RenderAllFaces();
    void RenderDepthOnly(Camera& cam);
    void RenderAllFacesShadowMapped(Camera& cam);
    void RenderDiffuse(Camera& cam);
    void RenderAllFacesW(Camera& cam);

    void InitFromPolyMesh(CPolyMesh& renderMesh);

    void PrepareMesh(CDisplayGL& displayGl, bool loadTextures);

    void SetRenderMode(NRenderMeshGL::ERenderMode renderMode)
    {
        m_renderMode = renderMode;
    }

    NRenderMeshGL::ERenderMode GetRenderMode()
    {
        return m_renderMode;
    }

    void LoadMeshTexures(CDisplayGL& displayGl);

    void Reset();

private:
    std::vector<NRenderMeshGL::Face> m_faces;
    unsigned int m_vaoId = 0;
    NRenderMeshGL::ERenderMode m_renderMode = NRenderMeshGL::ERenderDefault;

    void OnRenderStart();

    void OnRenderFinish();

    CDisplayGL* m_display{};

    Shader m_meshShader;

    glm::mat4 m_model = glm::mat4(1);

    bool m_hasLightmaps = false;

};
