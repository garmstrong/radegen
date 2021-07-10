#include <glad/glad.h>

#include "rendermesh_gl.h"
#include "osutils.h"
#include "display_gl.h"
#include "material.h"
#include "polymesh.h"
#include "camera.h"

using namespace NRenderTypes;

void CRenderMeshGL::OnRenderStart()
{
    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void CRenderMeshGL::OnRenderFinish()
{
    // restore all states
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
}

void CRenderMeshGL::Reset()
{
    glDeleteVertexArrays(1, &m_vaoId);

    // allocate mem for each new buffer
    for (auto & x : m_vertBuffers)
    {
        delete [] x.second.vertBuffer;
        x.second.copiedSoFar = 0;
        x.second.mat = nullptr;
        glDeleteBuffers(1, &x.second.glVBOId);
    }
    m_vertBuffers.clear();

    m_tmpFaces.clear();
    m_vaoId = 0;
    m_renderMode = NRenderTypes::ERenderDefault;
}

void CRenderMeshGL::RenderAllFaces(const rade::Camera& cam)
{
    OnRenderStart();

    m_meshShader.Use();
    m_meshShader.SetMat4("projection", cam.GetProjection());
    m_meshShader.SetMat4("view", cam.GetView());
    m_meshShader.SetMat4("model", m_model);

    for (auto & x : m_vertBuffers)
    {
        if (x.second.glVBOId == 0)
        {
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, x.second.glVBOId);

//        bool useFaceLightmaps = false;
//        if(x.second.lightmapID != 0 && m_hasLightmaps)
//        {
//            useFaceLightmaps = true;
//        }

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

//        if(useFaceLightmaps)
//        {
//            glVertexAttribPointer(
//                    4,                  // index
//                    2,                  // size
//                    GL_FLOAT,           // type
//                    GL_FALSE,           // normalised
//                    sizeof(Vert),       // stride
//                    &vert->texCoordLM);   // pointer
//        }

        glActiveTexture(GL_TEXTURE0);

        GLuint texID = 1;
        if(x.second.mat != nullptr)
        {
            using namespace RMaterials;
            texID = x.second.mat->GetTextureProps(TEXTURE_SLOT_DIFFUSE)->loadedTextureID;
        }
        glBindTexture(GL_TEXTURE_2D, texID);

        //glDrawArrays(GL_TRIANGLES, 0, x.second.numVerts);
        glDrawArrays(GL_TRIANGLE_FAN, 0, x.second.numVerts);
    }
    OnRenderFinish();
}

void CRenderMeshGL::AddFace(Face& face)
{
    m_tmpFaces.emplace_back(face);
}

void CRenderMeshGL::PrepareMesh(CDisplayGL& displayGl, bool loadTextures, bool usePlatformAssets /*= false*/)
{
    // make 1 big vert buffer
    for(Face& face : m_tmpFaces)
    {
        m_vertBuffers[face.materialKey].numVerts += static_cast<uint16_t>(face.verts.size());
    }

    // allocate mem for each new buffer
    for (auto & x : m_vertBuffers)
    {
        x.second.vertBuffer = new Vert[x.second.numVerts];
        x.second.copiedSoFar = 0;
        x.second.mat = nullptr;
    }

    // copy the vert info into the buffer structure
    for(Face& face : m_tmpFaces)
    {
        uint16_t copyIndex = 0;
        for(const auto& v : face.verts)
        {
            m_vertBuffers[face.materialKey].vertBuffer[m_vertBuffers[face.materialKey].copiedSoFar] =
                    v;
            m_vertBuffers[face.materialKey].copiedSoFar++;
            copyIndex++;
        }
    }

    // diffuse shader
    if (!m_meshShader.CreateShader(
            "data/shaders/vs.glsl",
            "data/shaders/fs.glsl"))
    {
        rade::Abort("Failed to create shader\n");
    }

    // generate vertex array id
    glGenVertexArrays(1, &m_vaoId);

    // generate VBO's for each section
    for (auto & x : m_vertBuffers)
    {
        glGenBuffers(1, &x.second.glVBOId);
        glBindBuffer(GL_ARRAY_BUFFER, x.second.glVBOId);
        glBufferData(GL_ARRAY_BUFFER,
                sizeof(Vert) * x.second.numVerts,
                &x.second.vertBuffer[0],
                GL_STATIC_DRAW);
        x.second.copiedSoFar = 0;
        x.second.materialName = x.first;
    }

    if(loadTextures)
    {
        LoadMeshTexures(displayGl, usePlatformAssets);
    }
    m_tmpFaces.clear();
}

void CRenderMeshGL::LoadMeshTexures(CDisplayGL& displayGl, bool usePlatformAssets /*= false*/)
{
    for (auto & x : m_vertBuffers)
    {
        if (x.second.mat == nullptr)
        {
            std::string materialKey = x.second.materialName;

            CMaterial* newMaterial = displayGl.GetMaterialMgr().LoadFromKey(materialKey);
            if(newMaterial)
            {
                x.second.mat = newMaterial;
            }
            else
            {
                rade::Log("Failed to load material %s\n", x.second.materialName.c_str());
            }
        }
    }
}

void CRenderMeshGL::InitFromPolyMesh(rade::CPolyMesh& polyMesh)
{
    std::vector<rade::CPoly3D>& polyList = polyMesh.GetPolyListRef();

    m_hasLightmaps = polyMesh.HasLightmaps();

    for (rade::CPoly3D& poly : polyList)
    {
        Face renderFace;
        renderFace.glVBOId = 0;
        renderFace.lightmapID = poly.GetLightTexID();

        for (auto& point : poly.GetPointListRef())
        {
            Vert vert;

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

