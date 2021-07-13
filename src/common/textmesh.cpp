#include <string>

#include "textmesh.h"
#include "point2d.h"
#include "display_gl.h"

namespace rade
{
    textmesh::textmesh()
    = default;

    bool textmesh::Init(const std::string& matKey,
            const uint16_t size,
            const uint16_t maxChars /*= 256*/ )
    {
        m_matKey = matKey;
        m_maxChars = maxChars;
        GenerateLabelGeometry(m_maxChars, size);
        return true;
    }

    bool textmesh::Reset()
    {
        m_matKey = "";
        return true;
    }

    rade::vector2 textmesh::GetGlyphPosition(char glyphChar)
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

    bool textmesh::GenerateLabelGeometry(const uint16_t maxChars, const uint16_t size)
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
            newPoly.SetShaderKey("font");
            m_polyMesh.AddPoly(newPoly);
        }

        return true;
    }

};
