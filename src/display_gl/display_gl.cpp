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

    //if (genMipMaps || GL_minMagFilter == GL_NEAREST_MIPMAP_LINEAR || GL_minMagFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    *id = texid;
    return true;
}

void CDisplayGL::RenderDebugQuad(Camera& cam)
{


//    float xoffset = -0.5;
//    float yoffset = -0.5;
//
//    float quadSize = 100.0f;
//
//    float quadVertices[] = {
//            // positions        // texture Coords
//             quadSize + xoffset, -quadSize + yoffset, 1.0f, // top right
//             quadSize + xoffset,  quadSize + yoffset, 1.0f, // bottom right
//            -quadSize + xoffset, -quadSize + yoffset, 1.0f, // bottom left
//            -quadSize + xoffset,  quadSize + yoffset, 1.0f, // top left
//    };
//
//    float quadTex[] = {
//            // texture Coords
//            1.0f, 0.0f,  // top right
//            1.0f, 1.0f,  // bottom right
//            0.0f, 0.0f,  // bottom left
//            0.0f, 1.0f,  // top left
//    };
//
//    glm::mat4 view(1);
//    glm::mat4 model(1);
//
//    m_spriteShader.Use();
//    m_spriteShader.SetMat4("projection", cam.GetProjection());
//    m_spriteShader.SetMat4("view", view);
//
//    glm::vec3 pos(0, 0, 0);
//    m_spriteShader.SetMat4("model", model);
//
//    glEnable(GL_TEXTURE_2D);
//
//    glBindTexture(GL_TEXTURE_2D, 1);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, quadVertices);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, quadTex);
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glDisable(GL_DEPTH_TEST);
//    glDrawArrays(GL_LINE_LOOP, 0, 4);
//    glEnable(GL_DEPTH_TEST);
//
//    glm::mat4 view(1);
//    glm::mat4 model(1);
//    m_spriteShader.Use();
//    m_spriteShader.SetMat4("projection", cam.GetProjection());
//    m_spriteShader.SetMat4("view", cam.GetView());
//
//    glm::vec3 pos(0, 0, 0);
//    m_spriteShader.SetMat4("model", model);
//
//    glEnable(GL_TEXTURE_2D);
//
//    float xoffset = -0.5;
//    float yoffset = -0.5;
//
//    float leftPos = -10.0f;
//    float rightPos = 10.0f;
//
//    if (m_quadVAO == 0)
//    {
//        float quadVertices[] = {
//                // positions        // texture Coords
//                leftPos + xoffset, rightPos + yoffset, 0.0f, 0.0f, 1.0f, // top left
//                leftPos + xoffset, leftPos + yoffset, 0.0f, 0.0f, 0.0f, // bottom left
//                rightPos + xoffset, rightPos + yoffset, 0.0f, 1.0f, 1.0f,  // bottom right
//                rightPos + xoffset, leftPos + yoffset, 0.0f, 1.0f, 0.0f,  // top right
//        };
//        // setup plane VAO
//        glGenVertexArrays(1, &m_quadVAO);
//        glGenBuffers(1, &m_quadVBO);
//        glBindVertexArray(m_quadVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    }
//
//    glBindVertexArray(m_quadVAO);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glBindVertexArray(0);
}

void CDisplayGL::SetViewport(int screenWidth, int screenHeight)
{
    m_videoWidth = screenWidth;
    m_videoHeight = screenHeight;
    glViewport(0, 0, screenWidth, screenHeight);
}

uint32_t CDisplayGL::AddMesh(rade::CPolyMesh& polyMesh)
{
    // generate a GL specific render mesh object for this mesh
    CMeshGL renderMesh;
    renderMesh.InitFromPolyMesh(polyMesh);
    renderMesh.PrepareMesh(*this);
    renderMesh.LoadMeshTexures(*this);
    m_meshes.push_back(renderMesh);
    return static_cast<uint32_t>(m_meshes.size()); // 0 = invalid
}

void CDisplayGL::RenderMeshID(uint32_t id, rade::Camera& cam)
{
    Assert(m_activeCamera, "Call SetCamera before rendering\n");
    if (m_meshes.size() < id)
        return;

    m_meshes.at(id - 1).RenderAllFaces(this);
}

void CDisplayGL::RenderMeshes(rade::Camera& cam)
{
    for(auto& mesh : m_meshes)
    {
        mesh.RenderAllFaces(this);
    }
}

void CDisplayGL::DeleteMesh(uint32_t id)
{
    if (m_meshes.size() < id)
        return;

    m_meshes.at(id - 1).Reset();

    m_meshes.erase(m_meshes.begin()+ id - 1);
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
