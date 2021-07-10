#pragma once

#include <vector>
#include <map>
#include "glm/glm.hpp"
#include "shader_gl.h"
#include "mesh_gl.h"
//#include "rendermesh_gl.h"
#include "rendertext_gl.h"
#include "camera.h"
#include "materialmanager.h"
#include "textmesh.h"

class CDisplayGL
{
public:
    CDisplayGL();

    virtual ~CDisplayGL();

    bool Init(int screenWidth, int screenHeight);

    void Shutdown();

    void SetActiveCamera(rade::Camera* camera);

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

    void RenderMeshID(uint32_t id, rade::Camera& cam);

    void RenderMeshes(rade::Camera& cam);

    uint32_t AddMesh(rade::CPolyMesh& polyMesh);

    int GetMaxTextureSize()
    {
        return m_maxTextureSize;
    }

    void DeleteMesh(uint32_t id);

    void RenderTextObjects();

    void LoadTextMesh(rade::CTextMesh* textMesh);

    void UpdateTextMesh(const std::string& name, const std::string& newString);

    void UpdateTextMeshPos(const std::string& name, const rade::vector3& pos);

    void UpdateTextMeshCamera(const std::string& name, rade::Camera* camera);

    void RemoveTextMesh(const std::string& name);

    static bool DeleteTextureID(uint32_t texID);

    void RenderDebugQuad(rade::Camera& cam);

private:
    unsigned int m_videoWidth = 800;
    unsigned int m_videoHeight = 600;
    bool m_captureMouse = false;
    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
    CMaterial* m_noTexture = nullptr;
    rade::Camera* m_activeCamera = nullptr;

    std::vector<CMeshGL> m_meshes;
    std::map<std::string, CRenderTextGL*> m_textMeshes;

    CMaterialManager m_materialMgr;

    void DrawDebug();


    int m_maxTextureSize = 1024;
    int m_maxTextureUnits = 16;

    Shader m_fontShader;

    Shader m_spriteShader;

};
