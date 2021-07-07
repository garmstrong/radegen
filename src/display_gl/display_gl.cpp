#include "display_gl.h"
#include "osutils.h"
#include "camera.h"
#include "material.h"

#include <glad/glad.h>

CDisplayGL::CDisplayGL() :
        m_materialMgr(*this)
{
}

CDisplayGL::~CDisplayGL()
{
    for (auto& textMesh : m_textMeshes)
    {
        delete textMesh.second;
    }
    m_textMeshes.clear();
}

bool CDisplayGL::Init(int screenWidth, int screenHeight)
{
    bool success = true;

    // print out some info about the graphics drivers
    OS::Log("OpenGL version: %s\n", glGetString(GL_VERSION));
    OS::Log("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    OS::Log("Vendor: %s\n", glGetString(GL_VENDOR));
    OS::Log("Renderer: %s\n", glGetString(GL_RENDERER));

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);

    SetViewport(screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_noTexture = m_materialMgr.LoadFromKey("notexture");
    if (!m_noTexture)
    {
        OS::Abort("Failed to load notexture, check working directory\n");
        success = false;
    }



    // font shader
    if (!m_fontShader.CreateShader(
            "data/shaders/font_vert.shader",
            "data/shaders/font_frag.shader"))
    {
        OS::Abort("Failed to create shader\n");
    }

    return success;
}

CMaterial* CDisplayGL::GetNoTexture()
{
    return m_noTexture;
}

void CDisplayGL::SetActiveCamera(Camera* camera)
{
    m_activeCamera = camera;
}

void CDisplayGL::Shutdown()
{

}

void CDisplayGL::DrawDebug()
{

}

void CDisplayGL::OnToggleDebug()
{

}

// update the scene based on the time elapsed since last update
void CDisplayGL::Draw(float deltaTime)
{
    glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawDebug();
}

bool CDisplayGL::LoadRAWTextureData(const unsigned char* data,
        const unsigned int width,
        const unsigned int height,
        const int channels,
        const bool genMipMaps,
        const RMaterials::ETextureFilterMode minMagFiler,
        const RMaterials::ETextureClampMode clampMode,
        uint32_t* id)
{
    // map RMaterials to GL
    int GL_minMagFilter = GL_NEAREST_MIPMAP_NEAREST;
    int GL_clampMode = GL_REPEAT;

    if(minMagFiler == RMaterials::TEXTURE_FILTER_LINEAR)
    {
        GL_minMagFilter = GL_LINEAR;
    }

    switch(clampMode)
    {
    case RMaterials::TEXTURE_REPEAT_CLAMP_TO_BORDER:
        GL_clampMode = GL_CLAMP_TO_BORDER;
        break;

    case RMaterials::TEXTURE_REPEAT_REPEAT:
        GL_clampMode = GL_REPEAT;
        break;

    case RMaterials::TEXTURE_REPEAT_CLAMP_TO_EDGE:
        GL_clampMode = GL_CLAMP_TO_EDGE;
        break;

    default:
        break;
    }

    uint32_t texid = 0;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_clampMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_clampMode);

    glTexImage2D(GL_TEXTURE_2D,
            0,
            channels == 3 ? GL_RGB : GL_RGBA,
            (GLsizei)width,
            (GLsizei)height,
            0,
            channels == 3 ? GL_RGB : GL_RGBA,
            GL_UNSIGNED_BYTE,
            data);

    if (genMipMaps || GL_minMagFilter == GL_NEAREST_MIPMAP_LINEAR || GL_minMagFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    *id = texid;
    return true;
}

void CDisplayGL::RenderDebugQuad()
{
    float xoffset = -0.5;
    float yoffset = -0.5;

    if (m_quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -0.5f + xoffset, 0.5f + yoffset, 0.0f, 0.0f, 1.0f, // top left
                -0.5f + xoffset, -0.5f + yoffset, 0.0f, 0.0f, 0.0f, // bottom left
                0.5f + xoffset, 0.5f + yoffset, 0.0f, 1.0f, 1.0f,  // bottom right
                0.5f + xoffset, -0.5f + yoffset, 0.0f, 1.0f, 0.0f,  // top right
        };
        // setup plane VAO
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void CDisplayGL::SetViewport(int screenWidth, int screenHeight)
{
    m_videoWidth = screenWidth;
    m_videoHeight = screenHeight;
    glViewport(0, 0, screenWidth, screenHeight);
}

uint32_t CDisplayGL::AddMesh(CPolyMesh& polyMesh)
{
    // CRenderDebugMeshGL is for debugging / editor modes only
    // this class does not group meshes by material, and renders each seperatly!
    // see CRenderMeshGL
    CRenderDebugMeshGL renderMesh;
    renderMesh.InitFromPolyMesh(polyMesh);

    bool loadTextures = true;
    renderMesh.PrepareMesh(*this, loadTextures);

    //renderMesh.LoadMeshTexures(*this);
    m_meshes.push_back(renderMesh);
    return m_meshes.size(); // 0 = invalid
}

void CDisplayGL::RenderMeshID(uint32_t id, Camera& cam)
{
    OS::Assert(m_activeCamera, "Call SetCamera before rendering\n");
    if (m_meshes.size() < id)
        return;

    m_meshes.at(id - 1).RenderDiffuse(cam);
}

void CDisplayGL::DeleteMesh(uint32_t id)
{
    OS::Assert(m_activeCamera, "Call SetCamera before rendering\n");
    if (m_meshes.size() < id)
        return;

    m_meshes.at(id - 1).Reset();

    m_meshes.erase(m_meshes.begin()+id);
}

void CDisplayGL::RenderAllTextObjects()
{
    //
    // TEXT MESHES
    //
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_fontShader.Use();
    for (auto& textMesh : m_textMeshes)
    {
        CRenderTextGL *txtGL = textMesh.second;
        m_fontShader.SetMat4("projection", txtGL->GetCamera()->GetProjection());
        m_fontShader.SetMat4("view", txtGL->GetCamera()->GetView());
        //m_fontShader.SetMat4("model", glm::mat4(1.0f));
        glm::vec3 pos(txtGL->GetPos().x, txtGL->GetPos().y, txtGL->GetPos().z);
        m_fontShader.SetMat4("model", glm::translate(glm::mat4(), pos));
        Camera* cam = txtGL->GetCamera();
        OS::Assert(cam, "textmesh camera was null\n");
        txtGL->RenderAllFaces(&m_fontShader);
    }
}

void CDisplayGL::RemoveTextMesh(const std::string& name)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->Reset();
    delete textGL;
    m_textMeshes.erase(name);
}

void CDisplayGL::LoadTextMesh(CTextMesh* textMesh)
{
    CRenderTextGL *newMesh = new CRenderTextGL(this, &textMesh->GetPolyMesh());
    // return index so can be rendered later
    m_textMeshes[textMesh->GetDisplayID()] = newMesh;
}

void CDisplayGL::UpdateTextMesh(const std::string& name, const std::string& newString)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->UpdateText(newString);
}

void CDisplayGL::UpdateTextMeshPos(const std::string& name, const CPoint3D& pos)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->SetPos(pos);
}

void CDisplayGL::UpdateTextMeshCamera(const std::string& name, Camera *camera)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->SetCamera(camera);
}

