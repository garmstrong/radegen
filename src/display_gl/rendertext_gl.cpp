#include <glad/glad.h>

#include "polygon3d.h"
#include "rendertext_gl.h"
#include "polymesh.h"
#include "osutils.h"
#include "display_gl.h"
#include "material.h"

using namespace rade;

using namespace NRenderTextGL;

CRenderTextGL::CRenderTextGL(CDisplayGL* display, rade::polymesh* polyMesh)
{
    // generate vertex array id for this mesh
    glGenVertexArrays(1, &m_vaoId);
    AllocateFromMesh(polyMesh);

    std::string matKey = "system/font";

    CMaterial* m_mat = display->GetMaterialMgr().LoadFromKey(matKey);
    Assert(m_mat, "Could not load font texture from key %s\n", matKey.c_str());

    m_texid = m_mat->GetTextureProps(RMaterials::TEXTURE_SLOT_DIFFUSE)->loadedTextureID;
    Assert(m_texid > 0, "CRenderTextGL() Loaded texture ID was invalid\n");
}

void CRenderTextGL::OnRenderStart()
{
    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void CRenderTextGL::OnRenderFinish()
{
    // restore all states
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindVertexArray(0);

    //glBindTexture(GL_TEXTURE_2D, 0);
    //glActiveTexture(0);

    //glUseProgram(0);
}

void CRenderTextGL::RenderAllFaces(CDisplayGL& display)
{
    Shader *shader = display.GetShader("font");
    rade::Assert(shader, "CMeshGL cant find shader font\n");

    shader->Use();
    shader->SetMat4("projection", m_camera->GetProjection());
    shader->SetMat4("view", m_camera->GetView());
    shader->SetMat4("model", m_transform.GetModelMatrix());



    OnRenderStart();
    glBindVertexArray(m_vaoId);

    // dont loop all faces, just use the ones we need to render the string
    // UVs are not stored - look them up each time and pass to shaders for fast dynamic rendering

    std::string testStr = GetText();

    //for (NRenderTextGL::Face& face : m_faces)
    for (size_t i = 0; i < GetText().length(); i++)
    {
        NRenderTextGL::Face& face = m_faces.at(i);
        char glyphChar = testStr.at(i);

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

        rade::vector2 uvOffset = GetGlyphPosition(glyphChar);
        shader->SetVec2("glyphOffset", uvOffset.x, uvOffset.y);

        glBindTexture(GL_TEXTURE_2D, m_texid);

        glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)face.verts.size());
    }
    OnRenderFinish();
}

void CRenderTextGL::AddFace(Face& face)
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

void CRenderTextGL::AllocateFromMesh(rade::polymesh* renderMesh)
{
    std::vector<poly3d> polyList = renderMesh->GetPolyListRef();

    for (poly3d& poly : polyList)
    {
        NRenderTextGL::Face renderFace;
        renderFace.glVBOId = 0;

        for (auto& point : poly.GetPointListRef())
        {
            NRenderTextGL::Vert vert;

            // normal
            vert.normal.x = poly.GetNormal().x;
            vert.normal.y = poly.GetNormal().y;
            vert.normal.z = poly.GetNormal().z;

            vert.position.x = point.x;
            vert.position.y = point.y;
            vert.position.z = point.z;

            vert.texCoord.x = point.u;
            vert.texCoord.y = point.v;

            renderFace.verts.emplace_back(vert);
        }
        //renderFace.materialID = poly.GetLoadedMaterialID();

        AddFace(renderFace);
    }
}

void CRenderTextGL::UpdateText(const std::string& newText)
{
    SetText(newText);
}

rade::vector2 CRenderTextGL::GetGlyphPosition(char glyphChar)
{
    rade::vector2 uvOffset;
    const int cFontWidth = 16;
    const int cFontHeight = 16;
    const float scalex = ((float)cFontWidth / (float)256);
    const float scaley = ((float)cFontHeight / (float)256);
    uvOffset.x = static_cast<float>(glyphChar % cFontWidth) * scalex;
    uvOffset.y = static_cast<float>(glyphChar / cFontHeight) * scaley;
    return uvOffset;
}

void CRenderTextGL::Reset()
{
    for (NRenderTextGL::Face& face : m_faces)
    {
        if (face.glVBOId != 0)
        {
            glDeleteBuffers(1, &face.glVBOId);
        }
    }
    glDeleteVertexArrays(1, &m_vaoId);

    m_texid = 0;
    SetText("");
    m_camera = nullptr;
    m_faces.clear();
    m_vaoId = 0;
    m_mat = nullptr; // TODO: delete?
}
