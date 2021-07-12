#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "camera.h"
#include "point2d.h"
#include "polygon3d.h"

class CDisplayGL;

namespace rade
{
    class polymesh;
};

class Shader;

class CMaterial;

namespace rade
{
    class Camera;
}

namespace NRenderTextGL
{
	typedef struct
	{
		float x;
		float y;
		float z;
	} vec3Float;

	typedef struct
	{
		float x;
		float y;
	} vec2Float;

	typedef struct
	{
		vec3Float position;
		vec2Float texCoord;
		vec3Float normal;
	} Vert;

	typedef struct
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} colour3UByte;

	typedef struct
	{
		unsigned int glVBOId;
		uint32_t materialID;
		std::vector<Vert> verts;
	} Face;

	enum ERenderMode
	{
		ERenderWireframe,
		ERenderDefault,
		ERenderNone,
	};
}

class CRenderTextGL
{
public:
	CRenderTextGL(CDisplayGL* display, rade::polymesh* renderMesh);

	~CRenderTextGL();

	void AddFace(NRenderTextGL::Face& face);

	void RenderAllFaces(Shader *shader);

	void AllocateFromMesh(rade::polymesh* renderMesh);


	void SetRenderMode(NRenderTextGL::ERenderMode renderMode)
	{
		m_renderMode = renderMode;
	}

	NRenderTextGL::ERenderMode GetRenderMode()
	{
		return m_renderMode;
	}

	void UpdateText(const std::string& newText);

	rade::vector3 GetPos() const
	{
		return m_pos;
	}

	void SetPos(const rade::vector3& pos)
	{
		m_pos = pos;
	}

	void SetCamera(rade::Camera *camera)
	{
		m_camera = camera;
	}

	rade::Camera* GetCamera()
	{
		return m_camera;
	}

    void Reset();

private:
    rade::vector2 GetGlyphPosition(char glyphChar);

	std::vector<NRenderTextGL::Face> m_faces;
	unsigned int m_vaoId = 0;
	NRenderTextGL::ERenderMode m_renderMode = NRenderTextGL::ERenderDefault;

	void OnRenderStart();

	void OnRenderFinish();

	CDisplayGL* m_display{};

    rade::vector3 m_pos;

	rade::Camera *m_camera = nullptr;

	std::string m_text;

	uint32_t m_texid = 0;
    CMaterial *m_mat = nullptr;
};
