#pragma once

#include <vector>
#include <string>

class CMaterial;

namespace NRenderTypes
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
        vec2Float texCoordLM;
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
		unsigned int glVBOId; // VBOID
		std::string materialKey;
        uint32_t lightmapID;
		//CGLTexture *texture;
		CMaterial *material;
		std::vector<Vert> verts;
	} Face;

	typedef struct
    {
        //unsigned int glVBOId; // VBOID
        std::string materialKey;
        std::string shaderKey;
        uint32_t lightmapID;
        //CGLTexture *texture;
        CMaterial *material;
        Vert verts[3];
    } Tri;

	enum ERenderMode
	{
		ERenderWireframe,
		ERenderDefault,
		ERenderNone,
	};
}
