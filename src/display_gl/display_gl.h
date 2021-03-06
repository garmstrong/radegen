#pragma once

#include <vector>
#include <map>

//#include "glm/glm.hpp"
#include "shader_gl.h"
#include "mesh_gl.h"
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

    void Draw(double deltaTime);

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

    //
    // Meshes
    //
    void RenderAllMeshes();

    IRenderObj* AddPolyMesh(rade::polymesh& polyMesh);

    void DeleteMesh(IRenderObj* renderObj);

    int GetMaxTextureSize() const
    {
        return m_maxTextureSize;
    }

    void RenderTextObjects();

    IRenderObj* AddTextMesh(rade::textmesh& textMesh);

    void DeleteTextMesh(IRenderObj* renderObj);

    static bool DeleteTextureID(uint32_t texID);

    bool LoadShader(const std::string& name, const std::string& vertString, const std::string& fragString);
    Shader* GetShader(const std::string& name);

private:
    unsigned int m_videoWidth = 800;
    unsigned int m_videoHeight = 600;
    bool m_captureMouse = false;
    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
    CMaterial* m_noTexture = nullptr;
    rade::Camera* m_activeCamera = nullptr;


    std::map<IRenderObj*, CMeshGL*> m_meshes;
    //std::map<std::string, CRenderTextGL*> m_textMeshes;
    std::map<IRenderObj*, CRenderTextGL*> m_textMeshes;

    CMaterialManager m_materialMgr;

    void DrawDebug();

    int m_maxTextureSize = 1024;
    int m_maxTextureUnits = 16;

    std::map<std::string, Shader*> m_shaders;
};
