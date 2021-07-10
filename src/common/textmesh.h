#pragma once

#include <string>

#include "point2d.h"
#include "polymesh.h"

class CMaterial;
class CDisplayGL;

namespace rade
{
    class Camera;

    class CTextMesh
    {
    public:
        CTextMesh();

        ~CTextMesh();

        bool Init(const std::string& id,
                CDisplayGL* display,
                Camera* cam,
                const rade::vector3& pos,
                uint16_t size,
                const std::string& matKey,
                uint16_t maxChars = 256);

        bool Reset();

        void SetText(const std::string& label);

        CPolyMesh& GetPolyMesh()
        {
            return m_polyMesh;
        }

        rade::vector3 GetPos() const
        {
            return m_pos;
        }

        void SetPos(const rade::vector3& pos);

        void SetCamera(Camera* cam);

        std::string GetDisplayID()
        {
            return m_textHandleID;
        }

    private:
        bool GenerateLabelGeometry(uint16_t maxChars, uint16_t size);

        static rade::vector2 GetGlyphPosition(char glyphChar);

        std::string m_label;
        std::string m_matKey;
        rade::vector3 m_pos;
        CPolyMesh m_polyMesh;
        uint16_t m_maxChars = 0;
        CDisplayGL* m_display = nullptr;
        std::string m_textHandleID;
        Camera* m_camera = nullptr;
    };
};
