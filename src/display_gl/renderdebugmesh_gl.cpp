#include "polygon3d.h"
#include "renderdebugmesh_gl.h"
#include "polymesh.h"
#include "material.h"
#include "osutils.h"
#include "display_gl.h"
#include "camera.h"

#include <glad/glad.h>

using namespace NRenderMeshGL;

CRenderDebugMeshGL::CRenderDebugMeshGL()
{
    // generate vertex array id
    glGenVertexArrays(1, &m_vaoId);
}

CRenderDebugMeshGL::~CRenderDebugMeshGL()
{
    glDeleteVertexArrays(1, &m_vaoId);
}

void CRenderDebugMeshGL::Reset()
{
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (face.glVBOId != 0)
        {
            glDeleteBuffers(1, &face.glVBOId);
        }
    }
    glDeleteVertexArrays(1, &m_vaoId);

    m_faces.clear();
    m_vaoId = 0;
    m_renderMode = NRenderMeshGL::ERenderDefault;
    m_model = glm::mat4(1);
    m_hasLightmaps = false;
}

void CRenderDebugMeshGL::OnRenderStart()
{
    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if(m_hasLightmaps)
    {
        glEnableVertexAttribArray(3);
    }
}

void CRenderDebugMeshGL::OnRenderFinish()
{
    // restore all states
    if(m_hasLightmaps)
    {
        glDisableVertexAttribArray(3);
    }
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(m_hasLightmaps)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(0);

    //glUseProgram(0);
}

void CRenderDebugMeshGL::RenderAllFacesShadowMapped(Camera& cam)
{
    OnRenderStart();

    glBindVertexArray(m_vaoId);
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (face.glVBOId == 0)
        {
            continue;
        }

        bool useFaceLightmaps = false;
        if(face.lightmapID != 0 && m_hasLightmaps)
        {
            useFaceLightmaps = true;
        }

        glBindBuffer(GL_ARRAY_BUFFER, face.glVBOId);

        // use an empty Vert for offset into member data
        Vert* vert = nullptr;
        glVertexAttribPointer(
                0,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->position);   // pointer

        glVertexAttribPointer(
                1,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->normal);     // pointer

        glVertexAttribPointer(
                2,                  // index
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->texCoord);   // pointer

        if(useFaceLightmaps)
        {
            glVertexAttribPointer(
                    4,                  // index
                    2,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalised
                    sizeof(Vert),       // stride
                    &vert->texCoordLM);   // pointer
        }

        GLuint texID = 1;
        if(face.material != nullptr)
        {
            using namespace RMaterials;
            texID = face.material->GetTextureProps(TEXTURE_SLOT_DIFFUSE)->loadedTextureID;
        }
        glBindTexture(GL_TEXTURE_2D, texID);

        if(useFaceLightmaps)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, face.lightmapID);
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, face.verts.size());
    }
    OnRenderFinish();
}

void CRenderDebugMeshGL::RenderDiffuse(Camera& cam)
{
    m_meshShader.Use();
    m_meshShader.SetMat4("projection", cam.GetProjection());
    m_meshShader.SetMat4("view", cam.GetView());
    m_meshShader.SetMat4("model", glm::mat4(1.0f));
    m_meshShader.SetVec3("viewPos", cam.GetPosition());
    m_meshShader.SetVec3("lightPos", glm::vec3(0, 0, 0));
    m_meshShader.SetVec3("lightColor", glm::vec3(1, 1, 1));
    m_meshShader.SetInt("lightmapTexture", 1);

    OnRenderStart();

    glBindVertexArray(m_vaoId);
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (face.glVBOId == 0)
        {
            continue;
        }

        bool useFaceLightmaps = false;
        if(face.lightmapID != 0 && m_hasLightmaps)
        {
            useFaceLightmaps = true;
        }

        glBindBuffer(GL_ARRAY_BUFFER, face.glVBOId);

        // use an empty Vert for offset into member data
        Vert* vert = nullptr;
        glVertexAttribPointer(
                0,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->position);   // pointer

        glVertexAttribPointer(
                1,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->normal);     // pointer

        glVertexAttribPointer(
                2,                  // index
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->texCoord);   // pointer

        if(useFaceLightmaps)
        {
            glVertexAttribPointer(
                    3,                  // index
                    2,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalised
                    sizeof(Vert),       // stride
                    &vert->texCoordLM);   // pointer
        }

        GLuint texID = 1;
        if(face.material != nullptr)
        {
            using namespace RMaterials;
            texID = face.material->GetTextureProps(TEXTURE_SLOT_DIFFUSE)->loadedTextureID;
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        if(useFaceLightmaps)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, face.lightmapID);
        }

        glDrawArrays(GL_TRIANGLE_FAN, 0, face.verts.size());
    }
    OnRenderFinish();
}

void CRenderDebugMeshGL::RenderDepthOnly(Camera& cam)
{
    OnRenderStart();

    glBindVertexArray(m_vaoId);
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (face.glVBOId == 0)
        {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, face.glVBOId);

        // use an empty Vert for offset into member data
        Vert* vert = nullptr;
        glVertexAttribPointer(
                0,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->position);   // pointer

        glVertexAttribPointer(
                1,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->normal);     // pointer

        glVertexAttribPointer(
                2,                  // index
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->texCoord);   // pointer

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, face.verts.size());
    }
    OnRenderFinish();
}

void CRenderDebugMeshGL::RenderAllFacesW(Camera& cam)
{
    OnRenderStart();

    m_meshShader.Use();
    m_meshShader.SetMat4("projection", cam.GetProjection());
    m_meshShader.SetMat4("view", cam.GetView());
    m_meshShader.SetMat4("model", m_model);

    glBindVertexArray(m_vaoId);
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (face.glVBOId == 0)
        {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, face.glVBOId);

        // use an empty Vert for offset into member data
        Vert* vert = nullptr;
        glVertexAttribPointer(
                0,                  // index
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalised
                sizeof(Vert),       // stride
                &vert->position);   // pointer

        //glLineWidth(1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, face.verts.size());
    }
    OnRenderFinish();
}

void CRenderDebugMeshGL::AddFace(Face& face)
{
    // generate a vbo for it
    glGenBuffers(1, &face.glVBOId);
    glBindBuffer(GL_ARRAY_BUFFER, face.glVBOId);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(Vert) * face.verts.size(),
            &face.verts[0],
            GL_STATIC_DRAW);

    m_faces.emplace_back(face);
}

void CRenderDebugMeshGL::InitFromPolyMesh(CPolyMesh& renderMesh)
{
    std::vector<CPoly3D> polyList = renderMesh.GetPolyListRef();
    m_hasLightmaps = renderMesh.HasLightmaps();

    for (CPoly3D& poly : polyList)
    {
        NRenderMeshGL::Face renderFace {};
        renderFace.glVBOId = 0;
        renderFace.lightmapID = poly.GetLightTexID();

        for (auto& point : poly.GetPointListRef())
        {
            NRenderMeshGL::Vert vert;

            // normal
            vert.normal.x = poly.GetNormal().x;
            vert.normal.y = poly.GetNormal().y;
            vert.normal.z = poly.GetNormal().z;

            vert.position.x = point.x;
            vert.position.y = point.y;
            vert.position.z = point.z;

            vert.texCoord.x = point.u;
            vert.texCoord.y = point.v;

            vert.texCoordLM.x = point.lmU;
            vert.texCoordLM.y = point.lmV;

            renderFace.verts.emplace_back(vert);
        }
        renderFace.materialKey = poly.GetMaterialKey();

        AddFace(renderFace);
    }
}

void CRenderDebugMeshGL::PrepareMesh(CDisplayGL& displayGl, bool loadTextures)
{
    // diffuse shader
    if (!m_meshShader.CreateShader(
            "data/shaders/diffuse_spec_vert.shader",
            "data/shaders/diffuse_spec_frag.shader"))
    {
        OS::Abort("Failed to create shader\n");
    }

    if(loadTextures)
    {
        LoadMeshTexures(displayGl);
    }
}

void CRenderDebugMeshGL::LoadMeshTexures(CDisplayGL& displayGl)
{
    for (NRenderMeshGL::Face& face : m_faces)
    {
        if (!face.materialKey.empty())
        {
            CMaterial* newMaterial = displayGl.GetMaterialMgr().LoadFromKey(face.materialKey);
            if(newMaterial)
            {
                face.material = newMaterial;
            }
            else
            {
                OS::Log("Failed to load material %s\n", face.materialKey.c_str());
            }
        }
    }
}

