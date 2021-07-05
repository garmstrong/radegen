#include "polymesh.h"
#include "osutils.h"
#include "material.h"
#include "materialmanager.h"

void CPolyMesh::AddPoly(const CPoly3D &poly)
{
	m_polyList.emplace_back(poly);
}

void CPolyMesh::AddPolyList(std::vector<CPoly3D>& polyList)
{
	for (CPoly3D& poly : polyList)
	{
		AddPoly(poly);
	}
}

bool CPolyMesh::LoadMaterials(CMaterialManager &materialMgr, const std::string& extraPath /* = "" */)
{
	using namespace RMaterials;

	OS::Assert(!m_polyList.empty(), "LoadMaterial called on mesh with 0 polygons\n");
    bool success = true;
	// generate
	for (CPoly3D& poly : m_polyList)
	{
        materialMgr.LoadFromKey(poly.GetMaterialKey());
	}
	return success;
}

void CPolyMesh::LoadLightmaps(CMaterialManager &materialMgr, std::vector<CLogicalLightmap>& lightmaps)
{
	OS::Assert(!m_polyList.empty(), "LoadLightmaps called, but no polygons loaded\n");

	// generate
	unsigned int counter = 0;
	for (auto& lm : lightmaps)
	{
		char strKey[32];
		sprintf(strKey, "mesh_lm_%d", counter);

		lightmapInfo_t lmInfo {};

		uint32_t texID = 0;

        bool genMipMaps = false;

        // CLAMP_TO_EDGE and no mip maps are required for Lightmaps (or they will bleed at edges)
        RMaterials::ETextureFilterMode filterMode = RMaterials::TEXTURE_FILTER_LINEAR;
        RMaterials::ETextureClampMode clampMode = RMaterials::TEXTURE_REPEAT_CLAMP_TO_EDGE;

        bool loaded = materialMgr.LoadRAWTextureData(
				lm.GetLightmapData()->data,
				lm.GetHeaderPtr()->width,
				lm.GetHeaderPtr()->height,
				4, // TODO: this should reduced to 3
                genMipMaps,
                filterMode,
                clampMode,
				&texID);
		if(!loaded)
		{
			texID = 0;
			OS::Log("failed to load RAW texture data in LoadLightmaps()\n");
		}

		lmInfo.width = lm.GetHeaderPtr()->width;
		lmInfo.height = lm.GetHeaderPtr()->height;
		lmInfo.channels = 4;
		lmInfo.texID = texID;

		m_lightmaps.push_back(lmInfo);
		counter++;
	}

	// assign the new texture ids back onto the polys
	if(!m_lightmaps.empty())
	{
		m_hasLightmaps = true;
		for (CPoly3D& poly : m_polyList)
		{
			uint32_t lightDataIndex = poly.GetLightmapDataIndex();
			uint32_t textureID = m_lightmaps.at(lightDataIndex).texID;

			poly.SetLightTexID(textureID);
		}
	}
}

void CPolyMesh::Clear()
{
	m_polyList.clear();
}
