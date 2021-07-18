#include <glad/glad.h>

#include "mesh_gl.h"
#include "osutils.h"
#include "display_gl.h"
#include "material.h"
#include "polymesh.h"
#include "timer.h"

using namespace NRenderTypes;

CMeshGL::CMeshGL(rade::polymesh& polymesh, CMaterialManager& matMgr)
{
    InitFromPolyMesh(polymesh);
    PrepareMesh();
    LoadMeshTexures(matMgr);
}

void CMeshGL::Reset()
{
    for (auto & x : m_vertBuffers)
    {
        delete [] x.second.vertBuffer;
        x.second.copiedSoFar = 0;
        x.second.mat = nullptr;
        glDeleteBuffers(1, &x.second.glVBOId);
        glDeleteVertexArrays(1, &x.second.glVAOId);
    }
    m_vertBuffers.clear();

    m_tmpFaces.clear();
    m_renderMode = NRenderTypes::ERenderDefault;
}

void CMeshGL::RenderAllFaces(CDisplayGL *display)
{
    rade::Assert(m_camera, "Camera is null\n");
    static rade::timer timer;

    // generate VBO's for each section
    for (auto & vbuff : m_vertBuffers)
    {
        Shader *shader = display->GetShader(vbuff.second.shaderName);
        rade::Assert(shader, "CMeshGL cant find shader %s\n", vbuff.second.shaderName.c_str());

        shader->Use();
        shader->SetMat4("projection", m_camera->GetProjection());
        shader->SetMat4("view", m_camera->GetView());
        shader->SetMat4("model", m_transform.GetMatrix());
        shader->SetVec3("viewPos", m_camera->GetTransform().GetPosition());
        shader->SetVec3("lightPos", rade::vector3(0, 0, 0));
        shader->SetVec3("lightColor", rade::vector3(1, 1, 1));
        shader->SetInt("lightmapTexture", 1);
        shader->SetFloat("time", timer.ElapsedTime());

        GLuint texID = 1;
        if(vbuff.second.mat != nullptr)
        {
            using namespace RMaterials;
            texID = vbuff.second.mat->GetTextureProps(TEXTURE_SLOT_DIFFUSE)->loadedTextureID;
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);

        if(vbuff.second.lightmapID != 0)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, vbuff.second.lightmapID);
        }

        glBindVertexArray(vbuff.second.glVAOId);
        glDrawArrays(GL_TRIANGLES, 0, vbuff.second.numVerts);

        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void CMeshGL::AddFace(Tri& face)
{
    m_tmpFaces.emplace_back(face);
}

void CMeshGL::PrepareMesh()
{
    // lightmapID really needs to be in a sprite sheet / atlas
    // make vertex buffer for each material key
    for(Tri& face : m_tmpFaces)
    {
        std::string matKey = face.materialKey + "_" + std::to_string(face.lightmapID)  + "_" + face.shaderKey;
        m_vertBuffers[matKey].numVerts += 3;// static_cast<uint16_t>(face.verts.size());
    }

    // allocate mem for each new buffer
    for (auto & x : m_vertBuffers)
    {
        x.second.vertBuffer = new Vert[x.second.numVerts];
        x.second.copiedSoFar = 0;
        x.second.mat = nullptr;
    }

    // copy the raw vert info into the buffer structure
    for(Tri& face : m_tmpFaces)
    {
        uint16_t copyIndex = 0;
        for(const auto& v : face.verts)
        {
            std::string matKey = face.materialKey + "_" + std::to_string(face.lightmapID)  + "_" + face.shaderKey;
            vertBuffer_t* vb = &m_vertBuffers[matKey];
            vb->vertBuffer[vb->copiedSoFar] = face.verts[copyIndex];
            vb->copiedSoFar++;
            vb->materialName = face.materialKey;
            vb->shaderName = face.shaderKey;
            vb->mat = face.material;
            vb->lightmapID = face.lightmapID;
            copyIndex++;
        }
    }

    // generate VAO and VBO for each section
    for (auto & x : m_vertBuffers)
    {
        // generate vertex array id
        glGenVertexArrays(1, &x.second.glVAOId);
        glGenBuffers(1, &x.second.glVBOId);
        glBindVertexArray(x.second.glVAOId);
        glBindBuffer(GL_ARRAY_BUFFER, x.second.glVBOId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * x.second.numVerts, &x.second.vertBuffer[0], GL_STATIC_DRAW);

        // use an empty Vert for offset into member data
        Vert* vert = nullptr;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), &vert->position);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), &vert->normal);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), &vert->texCoord);
        if(x.second.lightmapID != 0)
        {
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), &vert->texCoordLM);
            glEnableVertexAttribArray(3);
        }
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        x.second.copiedSoFar = 0;
    }

    m_tmpFaces.clear();
}

void CMeshGL::LoadMeshTexures(CMaterialManager& materialMgr, bool usePlatformAssets /*= false*/)
{
    for (auto & x : m_vertBuffers)
    {
        if (x.second.mat == nullptr)
        {
            std::string materialKey = x.second.materialName;
            CMaterial* newMaterial = materialMgr.LoadFromKey(materialKey);
            if(!newMaterial)
            {
                rade::Log("Failed to load material %s\n", x.second.materialName.c_str());
                x.second.mat = nullptr;
            }
            else
            {
                x.second.mat = newMaterial;
            }
        }
    }
}

void CMeshGL::InitFromPolyMesh(rade::polymesh& polyMesh)
{
    std::vector<rade::poly3d>& polyList = polyMesh.GetPolyListRef();
    m_hasLightmaps = polyMesh.HasLightmaps();
    for (rade::poly3d& poly : polyList)
    {
        std::vector<rade::poly3d> polyTriangles = poly.ToTriangles();
        for(auto& polyTri : polyTriangles)
        {
            Tri renderTri {};
            for(int i=0; i<3; i++)
            {
                Vert vert{};
                polyTri.GetPoint(i).ToRenderVert(&vert);
                vert.normal.x = poly.GetNormal().x;
                vert.normal.y = poly.GetNormal().y;
                vert.normal.z = poly.GetNormal().z;
                renderTri.verts[i] = vert;
            }
            renderTri.shaderKey = poly.GetShaderKey();
            renderTri.lightmapID = poly.GetLightTexID();
            renderTri.materialKey = poly.GetMaterialKey();
            AddFace(renderTri);
        }
    }
}

