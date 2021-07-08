#include <glad/glad.h>

#include "mesh_gl.h"
#include "osutils.h"
#include "display_gl.h"
#include "material.h"
#include "polymesh.h"
#include "camera.h"

using namespace NRenderTypes;

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

void CMeshGL::RenderAllFaces(const Camera& cam)
{
    // generate VBO's for each section
    for (auto & vbuff : m_vertBuffers)
    {
        m_meshShader.Use();
        m_meshShader.SetMat4("projection", cam.GetProjection());
        m_meshShader.SetMat4("view", cam.GetView());
        m_meshShader.SetMat4("model", glm::mat4(1.0f));
        m_meshShader.SetVec3("viewPos", cam.GetPosition());
        m_meshShader.SetVec3("lightPos", glm::vec3(0, 0, 0));
        m_meshShader.SetVec3("lightColor", glm::vec3(1, 1, 1));
        m_meshShader.SetInt("lightmapTexture", 1);

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

        glBindVertexArray(vbuff.second.glVAOId); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, vbuff.second.numVerts);
    }
}

void CMeshGL::AddFace(Tri& face)
{
    m_tmpFaces.emplace_back(face);
}

void CMeshGL::PrepareMesh(CDisplayGL& displayGl, bool loadTextures, bool usePlatformAssets /*= false*/)
{
    // lightmapID really needs to be in a sprite sheet / atlas
    // make vertex buffer for each material key
    for(Tri& face : m_tmpFaces)
    {
        std::string matKey = face.materialKey + "_" + std::to_string(face.lightmapID);
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
            std::string matKey = face.materialKey + "_" + std::to_string(face.lightmapID);

            m_vertBuffers[matKey].vertBuffer[m_vertBuffers[matKey].copiedSoFar] =
                    face.verts[copyIndex];
            m_vertBuffers[matKey].copiedSoFar++;

            m_vertBuffers[matKey].materialName = face.materialKey;
            m_vertBuffers[matKey].mat = face.material;
            m_vertBuffers[matKey].lightmapID = face.lightmapID;
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

    // allocate mem for each new buffer
    for (auto & x : m_vertBuffers)
    {
        delete [] x.second.vertBuffer;
    }

    // diffuse shader
    if (!m_meshShader.CreateShader(
            "data/shaders/diffuse_spec_vert.shader",
            "data/shaders/diffuse_spec_frag.shader"))
    {
        OS::Abort("Failed to create shader\n");
    }

    if(loadTextures)
    {
        LoadMeshTexures(displayGl, usePlatformAssets);
    }
    m_tmpFaces.clear();
}

void CMeshGL::LoadMeshTexures(CDisplayGL& displayGl, bool usePlatformAssets /*= false*/)
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
                OS::Log("Failed to load material %s\n", x.second.materialName.c_str());
            }
        }
    }
}

void CMeshGL::InitFromPolyMesh(CPolyMesh& polyMesh)
{
    std::vector<CPoly3D>& polyList = polyMesh.GetPolyListRef();
    m_hasLightmaps = polyMesh.HasLightmaps();
    for (CPoly3D& poly : polyList)
    {
        std::vector<CPoly3D> polyTriangles = poly.ToTriangles();
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
            renderTri.lightmapID = poly.GetLightTexID();
            renderTri.materialKey = poly.GetMaterialKey();
            AddFace(renderTri);
        }
    }
}

