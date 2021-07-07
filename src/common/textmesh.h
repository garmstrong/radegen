#pragma once

#include <string>

#include "point2d.h"
#include "polymesh.h"

class CMaterial;
class Camera;
class CDisplayGL;

class CTextMesh
{
public:
	CTextMesh();

	~CTextMesh();

	bool Init(CDisplayGL* display, Camera* cam, const CPoint3D& pos, uint16_t size, const std::string& matKey, const uint16_t maxChars = 256 );

	void SetText(const std::string& label);

	CPolyMesh& GetPolyMesh()
	{
		return m_polyMesh;
	}

	CPoint3D GetPos() const
	{
		return m_pos;
	}

	void SetPos(const CPoint3D& pos);

	void SetCamera(Camera* cam);

private:
	std::string m_label;
	std::string m_matKey;

	CPoint3D m_pos{};

	bool GenerateLabelGeometry(uint16_t maxChars, uint16_t size);

	static CPoint2D GetGlyphPosition(char glyphChar);

	//uint16_t m_numLines = 0;

	CPolyMesh m_polyMesh;

	uint16_t m_maxChars = 0;

    CDisplayGL* m_display = nullptr;

	uint32_t m_textHandleID = 0;

	Camera *m_camera = nullptr;
};
