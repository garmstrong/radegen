#pragma once

#include <string>

#include "point2d.h"
#include "polymesh.h"
#include "irenderobj.h"

class CMaterial;
class CDisplayGL;

namespace rade
{
    class Camera;

    class textmesh
    {
    public:
        textmesh();

        bool Init(const std::string& matKey,
                uint16_t size,
                uint16_t maxChars = 256);

        bool Reset();

        polymesh& GetPolyMesh()
        {
            return m_polyMesh;
        }

    private:
        bool GenerateLabelGeometry(uint16_t maxChars, uint16_t size);

        static rade::vector2 GetGlyphPosition(char glyphChar);

        std::string m_matKey;
        rade::vector3 m_pos;
        polymesh m_polyMesh;
        uint16_t m_maxChars = 0;
    };
};
