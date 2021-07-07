#ifndef _MESHFILE_H_
#define _MESHFILE_H_

#include <vector>
#include <cstring>
#include <algorithm>
#include "lightmapgen.h"
#include "lightmapimage.h"

#include "polygon3d.h"

namespace NMeshFile
{
    const int MATERIAL_NAME_LEN = 16;

    // no auto byte alignment (these structs are written to file)
#pragma pack(push, 1)
    typedef struct
    {
        uint8_t id;
        uint8_t version;
        uint32_t numFaces;
        uint32_t numMaterials;
        uint32_t numLightmaps;
        uint32_t numLights;
        char name[16];
    } SMeshHeader;

    typedef struct
    {
        char name[MATERIAL_NAME_LEN];
        float pos[3];
        float dir[3];
        float radius;
        float brightness;
        float color[3];
    } SLight;

    typedef struct
    {
        char materialName[MATERIAL_NAME_LEN];
        uint8_t numTextureKeys;
    } SMaterialHeader;

    typedef struct
    {
        char keyName[MATERIAL_NAME_LEN];     // ie, diffuse
        char fileName[MATERIAL_NAME_LEN];    // ie, water.png
    } STextureKey;

    typedef struct
    {
        uint32_t numPoints;
        uint32_t matIndex;
        uint32_t lmIndex;
    } SPolyHeader;

    typedef struct
    {
        float point[3];
        float nornmal[3];
        float uv0[2];
        float uv1[2];
        float uv2[2];
    } SPolyPoint;

    typedef struct
    {
        uint8_t compression;
        uint16_t width;
        uint16_t height;
        uint32_t dataSize;
        uint32_t compressedDataSize;
    } SLightmapHeader;

    typedef struct
    {
        unsigned char* data;
    } SLightmap;

#pragma pack(pop)

}

class CLogicalPolygon
{
public:
    CLogicalPolygon() = default;

    ~CLogicalPolygon() = default;

    void SetMaterialIndex(uint16_t matindex)
    {
        polyHeader.matIndex = matindex;
    }

    void SetLightmapDataIndex(uint16_t lmindex)
    {
        polyHeader.lmIndex = lmindex;
    }

    uint16_t GetLightmapDataIndex()
    {
        return polyHeader.lmIndex;
    }

    void AddPoint(NMeshFile::SPolyPoint point)
    {
        points.push_back(point);
        polyHeader.numPoints++;
    }

    NMeshFile::SPolyHeader* GetHeaderPtr()
    {
        return &polyHeader;
    }

    std::vector<NMeshFile::SPolyPoint>& GetPoints()
    {
        return points;
    }

protected:
    NMeshFile::SPolyHeader polyHeader{};
    std::vector<NMeshFile::SPolyPoint> points;
};

class CLogicalMaterial
{
public:
    CLogicalMaterial() = default;

    ~CLogicalMaterial() = default;

    void SetKey(const std::string& matkey)
    {
        strncpy(materialHeader.materialName, matkey.c_str(), NMeshFile::MATERIAL_NAME_LEN);
    }

    void AddTexture(const std::string& keyName, const std::string& filename)
    {
        NMeshFile::STextureKey textureID;
        strncpy(textureID.keyName, keyName.c_str(), NMeshFile::MATERIAL_NAME_LEN);
        strncpy(textureID.fileName, filename.c_str(), NMeshFile::MATERIAL_NAME_LEN);
        textureKeys.push_back(textureID);
        materialHeader.numTextureKeys++;
    }

    std::string GetMaterialKey() const
    {
        return materialHeader.materialName;
    }

    NMeshFile::SMaterialHeader* GetHeaderPtr()
    {
        return &materialHeader;
    }

    std::vector<NMeshFile::STextureKey>& GetTextureKeys()
    {
        return textureKeys;
    }

protected:
    NMeshFile::SMaterialHeader materialHeader{};
    std::vector<NMeshFile::STextureKey> textureKeys;
};

class CLogicalLightmap
{
public:
    CLogicalLightmap() = default;

    ~CLogicalLightmap() = default;

    void
    AddLightmapData(const uint16_t width, const uint16_t height, unsigned char* dataPtr, const unsigned int dataSize)
    {
        lmHeader.width = width;
        lmHeader.height = height;
        lmHeader.dataSize = dataSize;
        lmData.data = new unsigned char[width * height * 4];
        //memset(lmData.data, 0, width * height * 4);
        memcpy(lmData.data, dataPtr, width * height * 4);
    }

    NMeshFile::SLightmapHeader* GetHeaderPtr()
    {
        return &lmHeader;
    }

    NMeshFile::SLightmap* GetLightmapData()
    {
        return &lmData;
    }

protected:
    NMeshFile::SLightmapHeader lmHeader{};
    NMeshFile::SLightmap lmData{};
};

class CMeshFile
{
public:

    explicit CMeshFile(const std::vector<CPoly3D>& polylist)
    {
        LoadFromPolyList(polylist);
    }

    CMeshFile();

    ~CMeshFile();

    bool LoadFromFile(const std::string& filename);

    void LoadFromPolyList(const std::vector<CPoly3D>& polylist);

    bool WriteToFile(const std::string& filename);

    void GetAsPolyList(std::vector<CPoly3D>& polyListOut);

    void AddLightmapData(
            uint16_t width,
            uint16_t height,
            unsigned char* dataPtr,
            unsigned int dataSize);

    void GetLightMaps(std::vector<CLightmapImg>& lmaps);

    size_t GetPolyCount()
    {
        return m_polygons.size();
    }

    void DeleteLightmapData();

    void AddLight(const CLight &light);

    std::vector<NMeshFile::SLight>& GetLightsRef()
    {
        return m_lights;
    }

protected:

    unsigned char* CreateCompressedBuffer(
            unsigned char* pData,
            unsigned long nDataSize,
            unsigned long* nCompressedDataSize);

    unsigned char* CreateUncompressedBuffer(unsigned char* pCompressedData,
            unsigned long nCompressedDataSize,
            unsigned long nOriginalDataSize);

    NMeshFile::SMeshHeader m_header{};
    std::vector<CLogicalMaterial> m_materials;
    std::vector<CLogicalPolygon> m_polygons;
    std::vector<CLogicalLightmap> m_lightmaps;

    std::vector<NMeshFile::SLight> m_lights;

    void FreeAll();

    void Reset();

    bool ValidateData();

    void AddPoly(const CPoly3D& poly);

    uint32_t AddDiffuseOnlyMaterial(const std::string& matkey, const std::string& diffuseName);

};

#endif
