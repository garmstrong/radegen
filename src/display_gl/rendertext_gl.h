#pragma once

#include <vector>
#include <cstdint>
#include <string>

#include "camera.h"
#include "point2d.h"
#include "polygon3d.h"

class CDisplayGL;

class CGLTexture;

class CPolyMesh;

class GLMaterial;

class Shader;

class CMaterial;

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
	CRenderTextGL(CDisplayGL* display, CPolyMesh* renderMesh);

	~CRenderTextGL();

	void AddFace(NRenderTextGL::Face& face);

	void RenderAllFaces(Shader *shader);

	void AllocateFromMesh(CPolyMesh* renderMesh);

	void Init(CDisplayGL* display);

	bool InitFromPolyList(std::vector<CPoly3D>& polyList);

	void SetRenderMode(NRenderTextGL::ERenderMode renderMode)
	{
		m_renderMode = renderMode;
	}

	NRenderTextGL::ERenderMode GetRenderMode()
	{
		return m_renderMode;
	}

	void UpdateText(const std::string& newText);

	CPoint3D GetPos() const
	{
		return m_pos;
	}

	void SetPos(const CPoint3D& pos)
	{
		m_pos = pos;
	}

	void SetCamera(Camera *camera)
	{
		m_camera = camera;
	}

	Camera* GetCamera()
	{
		return m_camera;
	}


private:
	CPoint2D GetGlyphPosition(char glyphChar);

	std::vector<NRenderTextGL::Face> m_faces;
	unsigned int m_vaoId = 0;
	NRenderTextGL::ERenderMode m_renderMode = NRenderTextGL::ERenderDefault;

	void OnRenderStart();

	void OnRenderFinish();

	CDisplayGL* m_display{};

	CPoint3D m_pos;

	Camera *m_camera = nullptr;

	std::string m_text;

	uint32_t m_texid = 0;
    CMaterial *m_mat = nullptr;
};
