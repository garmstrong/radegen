#pragma once

#include <vector>
#include <map>
#include "glm/glm.hpp"
#include "shader_gl.h"
#include "renderdebugmesh_gl.h"
//#include "rendermesh_gl.h"
#include "rendertext_gl.h"
#include "camera.h"
#include "materialmanager.h"
#include "textmesh.h"

class OS;

class CDisplayGL
{
public:
    CDisplayGL();

    virtual ~CDisplayGL();

    bool Init(int screenWidth, int screenHeight);

    void Shutdown();

    void SetActiveCamera(Camera* camera);

    void Draw(float deltaTime);

    bool LoadRAWTextureData(const unsigned char* data,
            unsigned int width,
            unsigned int height,
            int channels,
            bool genMipMaps,
            RMaterials::ETextureFilterMode minMagFiler,
            RMaterials::ETextureClampMode clampMode,
            uint32_t* id);

    void OnToggleDebug();

    void SetViewport(int screenWidth, int screenHeight);

    CMaterial* GetNoTexture();

    CMaterialManager& GetMaterialMgr()
    {
        return m_materialMgr;
    }

    void RenderMeshID(uint32_t id, Camera& cam);

    uint32_t AddMesh(CPolyMesh& polyMesh);

    int GetMaxTextureSize()
    {
        return m_maxTextureSize;
    }

    void DeleteMesh(uint32_t id);

    void RenderAllTextObjects();


    uint32_t LoadTextMesh(CTextMesh* textMesh);

    void UpdateTextMesh(uint32_t handleID, const std::string& newString);

    void UpdateTextMeshPos(uint32_t handleID, const CPoint3D& pos);

    void UpdateTextMeshCamera(uint32_t handleID, Camera* camera);

private:
    unsigned int m_videoWidth = 800;
    unsigned int m_videoHeight = 600;
    bool m_captureMouse = false;
    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
    CMaterial* m_noTexture = nullptr;
    Camera* m_activeCamera = nullptr;

    std::vector<CRenderDebugMeshGL> m_meshes;
    std::vector<CRenderTextGL> m_textMeshes;

    CMaterialManager m_materialMgr;

    void DrawDebug();

    void RenderDebugQuad();

    int m_maxTextureSize = 1024;
    int m_maxTextureUnits = 16;

    Shader m_fontShader;

};
