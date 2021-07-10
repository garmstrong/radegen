#include <string>

#include "textmesh.h"
#include "point2d.h"
#include "osutils.h"
#include "display_gl.h"

namespace rade
{
    CTextMesh::CTextMesh()
    = default;

    CTextMesh::~CTextMesh()
    = default;

    bool CTextMesh::Init(const std::string& id,
            CDisplayGL* display,
            Camera* cam,
            const rade::vector3& pos,
            const uint16_t size,
            const std::string& matKey,
            const uint16_t maxChars /*= 256*/ )
    {
        m_textHandleID = id;
        m_matKey = matKey;
        m_display = display;
        rade::Assert(m_display, "CTextMesh::RegisterWithDisplay Display cannot be null\n");
        m_label = "CTextMesh text";
        m_maxChars = maxChars;
        GenerateLabelGeometry(m_maxChars, size);
        display->LoadTextMesh(this);
        SetPos(pos);
        SetCamera(cam);
        return true;
    }

    bool CTextMesh::Reset()
    {
        m_display->RemoveTextMesh(m_textHandleID);
        m_matKey = "";
        m_display = nullptr;
        m_label = "CTextMesh text";
        m_textHandleID = "";
        return true;
    }

    void CTextMesh::SetText(const std::string& label)
    {
        if (m_label != label)
        {
            m_label = label;
            m_display->UpdateTextMesh(m_textHandleID, label);
        }
    }

    rade::vector2 CTextMesh::GetGlyphPosition(char glyphChar)
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

    bool CTextMesh::GenerateLabelGeometry(const uint16_t maxChars, const uint16_t size)
    {
        float size_x = size;
        float size_y = size;
        const int cFontWidth = 16;
        const int cFontHeight = 16;

        // draw the string with some made up quads
        float xOff, yOff = 0;
        const float scalex = ((float)cFontWidth / (float)256);
        const float scaley = ((float)cFontHeight / (float)256);

//	m_numLines = 0;

        m_polyMesh.Reset();

        float xCaretPos = 0.0f;
        for (unsigned int i = 0; i < maxChars; i++)
        {
            poly3d newPoly;

//		if (newChar == '\n')
//		{
//			yOff += size_y;
//			xCaretPos = 0;
//			m_numLines++;
//			continue;
//		}

            xOff = (xCaretPos * size_x);
            xCaretPos++;

            float width = size_x;
            float height = size_y;

            auto p1 = rade::vector3(xOff, yOff, -0.2f);
            auto p2 = rade::vector3(xOff, yOff + height, -0.2f);
            auto p3 = rade::vector3(xOff + width, yOff + height, -0.2f);
            auto p4 = rade::vector3(xOff + width, yOff, -0.2f);

            // initial offsets for polygon (needs above uv offset per glyph in vert shader to be show correct char image)
            p1.u = 0;
            p1.v = scaley;

            p2.u = 0;
            p2.v = 0;

            p3.u = scalex;
            p3.v = 0;

            p4.u = scalex;
            p4.v = scaley;

            newPoly.AddPoint(p4);
            newPoly.AddPoint(p3);
            newPoly.AddPoint(p2);
            newPoly.AddPoint(p1);
            newPoly.SetMaterialKey(m_matKey);
            m_polyMesh.AddPoly(newPoly);
        }

        return true;
    }

    void CTextMesh::SetPos(const rade::vector3& pos)
    {
        m_pos = pos;
        m_display->UpdateTextMeshPos(m_textHandleID, pos);
    }

    void CTextMesh::SetCamera(rade::Camera* cam)
    {
        m_camera = cam;
        m_display->UpdateTextMeshCamera(m_textHandleID, m_camera);
    }

};
