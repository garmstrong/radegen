#pragma once

#include <vector>
#include <cstring>
#include <algorithm>
#include "lightmapgen.h"
#include "lightmapimage.h"
#include "polygon3d.h"

namespace rade
{
    namespace mesh
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
    }; // namespace mesh

    class LogicalPolygon
    {
    public:
        LogicalPolygon() = default;

        ~LogicalPolygon() = default;

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

        void AddPoint(mesh::SPolyPoint point)
        {
            points.push_back(point);
            polyHeader.numPoints++;
        }

        mesh::SPolyHeader* GetHeaderPtr()
        {
            return &polyHeader;
        }

        std::vector<mesh::SPolyPoint>& GetPoints()
        {
            return points;
        }

    protected:
        mesh::SPolyHeader polyHeader{};
        std::vector<mesh::SPolyPoint> points;
    };

    class LogicalMaterial
    {
    public:
        LogicalMaterial() = default;

        ~LogicalMaterial() = default;

        void SetKey(const std::string& matkey)
        {
            strncpy(materialHeader.materialName, matkey.c_str(), mesh::MATERIAL_NAME_LEN);
        }

        void AddTexture(const std::string& keyName, const std::string& filename)
        {
            mesh::STextureKey textureID;
            strncpy(textureID.keyName, keyName.c_str(), mesh::MATERIAL_NAME_LEN);
            strncpy(textureID.fileName, filename.c_str(), mesh::MATERIAL_NAME_LEN);
            textureKeys.push_back(textureID);
            materialHeader.numTextureKeys++;
        }

        std::string GetMaterialKey() const
        {
            return materialHeader.materialName;
        }

        mesh::SMaterialHeader* GetHeaderPtr()
        {
            return &materialHeader;
        }

        std::vector<mesh::STextureKey>& GetTextureKeys()
        {
            return textureKeys;
        }

    protected:
        mesh::SMaterialHeader materialHeader{};
        std::vector<mesh::STextureKey> textureKeys;
    };

    class LogicalLightmap
    {
    public:
        LogicalLightmap() = default;

        ~LogicalLightmap() = default;

        void
        AddLightmapData(const uint16_t width, const uint16_t height, unsigned char* dataPtr,
                const unsigned int dataSize)
        {
            lmHeader.width = width;
            lmHeader.height = height;
            lmHeader.dataSize = dataSize;
            lmData.data = new unsigned char[width * height * 4];
            memcpy(lmData.data, dataPtr, width * height * 4);
        }

        mesh::SLightmapHeader* GetHeaderPtr()
        {
            return &lmHeader;
        }

        mesh::SLightmap* GetLightmapData()
        {
            return &lmData;
        }

    protected:
        mesh::SLightmapHeader lmHeader{};
        mesh::SLightmap lmData{};
    };

    class MeshFile
    {
    public:

        explicit MeshFile(const std::vector<poly3d>& polylist)
        {
            LoadFromPolyList(polylist);
        }

        MeshFile();

        ~MeshFile();

        bool LoadFromFile(const std::string& filename);

        void LoadFromPolyList(const std::vector<poly3d>& polylist);

        bool WriteToFile(const std::string& filename);

        void GetAsPolyList(std::vector<poly3d>& polyListOut);

        void AddLightmapData(
                uint16_t width,
                uint16_t height,
                unsigned char* dataPtr,
                unsigned int dataSize);

        void GetLightMaps(std::vector<CLightmapImg*>& lmaps);

        size_t GetPolyCount()
        {
            return m_polygons.size();
        }

        void DeleteLightmapData();

        void AddLight(const Light& light);

        std::vector<mesh::SLight>& GetLightsRef()
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

        void FreeAll();

        void Reset();

        bool ValidateData();

        void AddPoly(const poly3d& poly);

        uint32_t AddDiffuseOnlyMaterial(const std::string& matkey, const std::string& diffuseName);

        mesh::SMeshHeader m_header{};
        std::vector<LogicalMaterial> m_materials;
        std::vector<LogicalPolygon> m_polygons;
        std::vector<LogicalLightmap> m_lightmaps;
        std::vector<mesh::SLight> m_lights;

    };
};
