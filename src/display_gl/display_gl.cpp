#include "display_gl.h"
#include "osutils.h"
#include "camera.h"
#include "material.h"

#include <glad/glad.h>

using namespace rade;

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

    for (auto& shader : m_shaders)
    {
        delete shader.second;
    }
    m_shaders.clear();

    for (auto& mesh : m_meshes)
    {
        mesh.second->Reset();
        delete mesh.second;
    }
    m_meshes.clear();
}

bool CDisplayGL::Init(int screenWidth, int screenHeight)
{
    bool success = true;

    // print out some info about the graphics drivers
    Log("OpenGL version: %s\n", glGetString(GL_VERSION));
    Log("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    Log("Vendor: %s\n", glGetString(GL_VENDOR));
    Log("Renderer: %s\n", glGetString(GL_RENDERER));

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_maxTextureSize);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);

    SetViewport(screenWidth, screenHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_noTexture = m_materialMgr.LoadFromKey("notexture");
    if (!m_noTexture)
    {
        Abort("Failed to load notexture, check working directory\n");
        success = false;
    }

    return success;
}

CMaterial* CDisplayGL::GetNoTexture()
{
    return m_noTexture;
}

void CDisplayGL::SetActiveCamera(rade::Camera* camera)
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

bool CDisplayGL::DeleteTextureID(uint32_t texID)
{
    glDeleteTextures(1, &texID);
    return true;
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
    int GL_minMagFilter = GL_LINEAR_MIPMAP_LINEAR;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

void CDisplayGL::SetViewport(int screenWidth, int screenHeight)
{
    m_videoWidth = screenWidth;
    m_videoHeight = screenHeight;
    glViewport(0, 0, screenWidth, screenHeight);
}

IRenderObj* CDisplayGL::AddMesh(rade::polymesh& polyMesh)
{
    rade::Assert(m_activeCamera, "display has no active camera\n");

    // generate a GL specific render mesh object for this mesh
    auto* renderMesh = new CMeshGL(polyMesh, GetMaterialMgr());
    renderMesh->SetCamera(m_activeCamera);

    // the std::map really does nothing here, we are just hiding the CMeshGL (OpenGL specific) class
    // from the outside. It *could* be casted to the CMeshGL (for debugging purposes), but we dont
    // actually do that for normal run-time operations because the dynamic_cast is actually slow (and
    // badly with classes that have lots of data on them)
    auto* renderObj = dynamic_cast<IRenderObj*>(renderMesh);
    m_meshes[renderObj] = renderMesh;
    return renderObj;
}

void CDisplayGL::RenderAllMeshes()
{
    for(auto& mesh : m_meshes)
    {
        mesh.second->RenderAllFaces(this);
    }
}

void CDisplayGL::DeleteMesh(IRenderObj* renderObj)
{
    CMeshGL* meshGL = m_meshes[renderObj];
    rade::Assert(meshGL, "renderObj pointer is invalid or stale\n");

    meshGL->Reset();
    m_meshes.erase(renderObj);
    delete renderObj;
    renderObj = nullptr;
}

void CDisplayGL::RenderTextObjects()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader *shader = GetShader("font");
    rade::Assert(shader, "CMeshGL cant find font shader");

    shader->Use();
    for (auto& textMesh : m_textMeshes)
    {
        CRenderTextGL *txtGL = textMesh.second;
        shader->SetMat4("projection", txtGL->GetCamera()->GetProjection());
        shader->SetMat4("view", txtGL->GetCamera()->GetView());
        //m_fontShader.SetMat4("model", glm::mat4(1.0f));
        glm::vec3 pos(txtGL->GetPos().x, txtGL->GetPos().y, txtGL->GetPos().z);
        shader->SetMat4("model", glm::translate(glm::mat4(), pos));
        rade::Camera* cam = txtGL->GetCamera();
        Assert(cam, "textmesh camera was null\n");
        txtGL->RenderAllFaces(shader);
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
    auto *newMesh = new CRenderTextGL(this, &textMesh->GetPolyMesh());
    // return index so can be rendered later
    m_textMeshes[textMesh->GetDisplayID()] = newMesh;
}

void CDisplayGL::UpdateTextMesh(const std::string& name, const std::string& newString)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->UpdateText(newString);
}

void CDisplayGL::UpdateTextMeshPos(const std::string& name, const rade::vector3& pos)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->SetPos(pos);
}

void CDisplayGL::UpdateTextMeshCamera(const std::string& name, rade::Camera *camera)
{
    CRenderTextGL* textGL = m_textMeshes[name];
    textGL->SetCamera(camera);
}

bool CDisplayGL::LoadShader(const std::string& name, const std::string& vertFile, const std::string& fragFile)
{
    Shader *shader = new Shader();
    bool success = shader->CreateShader(name, vertFile.c_str(), fragFile.c_str());
    if(!success)
    {
        rade::Log("Shader %s failed to load\n", name.c_str());
        return false;
    }

    m_shaders[name] = shader;
    return success;
}

Shader* CDisplayGL::GetShader(const std::string& name)
{
    return m_shaders[name];
}
