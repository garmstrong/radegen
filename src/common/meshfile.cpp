#include <memory.h>
#include <cstdio>

#include "meshfile.h"
#include "miniz.h"
#include "osutils.h"

namespace rade
{
    MeshFile::MeshFile()
    {
        memset(&m_header, 0, sizeof(mesh::SMeshHeader));
    }

    MeshFile::~MeshFile()
    {
        FreeAll();
    }

    unsigned char* MeshFile::CreateCompressedBuffer(
            unsigned char* pData,
            unsigned long nDataSize,
            unsigned long* nCompressedDataSize)
    {
        *nCompressedDataSize = nDataSize;
        auto* pCompressedData = new unsigned char[*nCompressedDataSize];
        int nResult = compress(pCompressedData, nCompressedDataSize, pData, nDataSize);
        if (nResult != Z_OK)
        {
            return nullptr;
        }
        return pCompressedData;
    }

    unsigned char* MeshFile::CreateUncompressedBuffer(unsigned char* pCompressedData,
            unsigned long nCompressedDataSize,
            unsigned long nOriginalDataSize)
    {
        auto* pUncompressedData = new unsigned char[nOriginalDataSize];
        int nResult = uncompress(pUncompressedData, &nOriginalDataSize, pCompressedData, nCompressedDataSize);
        if (nResult != Z_OK)
        {
            return nullptr;
        }
        return pUncompressedData;
    }

    void MeshFile::FreeAll()
    {
        DeleteLightmapData();
    }

    void MeshFile::DeleteLightmapData()
    {
        for (auto & m_lightmap : m_lightmaps)
        {
            unsigned char* dataPtr = m_lightmap.GetLightmapData()->data;
            if (dataPtr)
            {
                delete[] dataPtr;
                m_lightmap.GetLightmapData()->data = nullptr;
            }
        }
        m_lightmaps.clear();

        m_header.numLightmaps = 0;
    }

    bool MeshFile::LoadFromFile(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "rb");
        if (!fp)
        {
            return false;
        }

        Reset();

        fread((mesh::SMeshHeader*)&m_header, sizeof(mesh::SMeshHeader), 1, fp);

        // materials
        for (uint32_t i = 0; i < m_header.numMaterials; i++)
        {
            LogicalMaterial mat;

            mesh::SMaterialHeader matHeader{};
            fread((mesh::SMaterialHeader*)&matHeader, sizeof(mesh::SMaterialHeader), 1, fp);
            mat.SetKey(matHeader.materialName);

            for (int j = 0; j < matHeader.numTextureKeys; j++)
            {
                mesh::STextureKey tex{};
                fread((mesh::STextureKey*)&tex, sizeof(mesh::STextureKey), 1, fp);
                mat.AddTexture(tex.keyName, tex.fileName);
            }
            m_materials.push_back(mat);
        }

        // polys
        for (uint32_t i = 0; i < m_header.numFaces; i++)
        {
            LogicalPolygon poly;

            mesh::SPolyHeader polyHeader{};
            fread((mesh::SPolyHeader*)&polyHeader, sizeof(mesh::SPolyHeader), 1, fp);
            poly.SetMaterialIndex(polyHeader.matIndex);
            poly.SetLightmapDataIndex(polyHeader.lmIndex);

            for (uint32_t j = 0; j < polyHeader.numPoints; j++)
            {
                mesh::SPolyPoint point;
                fread((mesh::SPolyPoint*)&point, sizeof(mesh::SPolyPoint), 1, fp);
                poly.AddPoint(point);
            }
            m_polygons.push_back(poly);
        }

        // lightmaps
        for (uint32_t i = 0; i < m_header.numLightmaps; i++)
        {
            LogicalLightmap newLightmap;

            size_t lmSize = sizeof(mesh::SLightmapHeader);

            mesh::SLightmapHeader lmHeader;
            fread((mesh::SLightmapHeader*)&lmHeader, sizeof(mesh::SLightmapHeader), 1, fp);

            auto* compressedBuffer = new unsigned char[lmHeader.compressedDataSize];
            fread((unsigned char*)compressedBuffer, lmHeader.compressedDataSize, 1, fp);

            mesh::SLightmap lightmap;
            lightmap.data = CreateUncompressedBuffer(compressedBuffer, lmHeader.compressedDataSize, lmHeader.dataSize);
            delete[] compressedBuffer;

            newLightmap.AddLightmapData(lmHeader.width, lmHeader.height, lightmap.data, lmHeader.dataSize);
            delete[] lightmap.data;

            m_lightmaps.push_back(newLightmap);
        }

        for (uint32_t i = 0; i < m_header.numLights; i++)
        {
            mesh::SLight light;
            fread((mesh::SLight*)&light, sizeof(mesh::SLight), 1, fp);
            m_lights.push_back(light);
        }
        ValidateData();
        fclose(fp);
        return true;
    }

    void MeshFile::LoadFromPolyList(const std::vector<poly3d>& polylist)
    {
        Reset();
        for (const poly3d& poly : polylist)
        {
            AddPoly(poly);
        }
        ValidateData();
    }

    void MeshFile::AddPoly(const poly3d& poly)
    {
        LogicalPolygon newPoly{};

        // use key bor material name and diffuse tex
        uint16_t matIndex = AddDiffuseOnlyMaterial(poly.GetMaterialKey(), poly.GetMaterialKey());
        newPoly.SetMaterialIndex(matIndex);
        newPoly.SetLightmapDataIndex(poly.GetLightmapDataIndex());

        const std::vector<rade::vector3>& pointsRef = poly.GetPointListRefConst();
        for (const auto& p : pointsRef)
        {
            mesh::SPolyPoint newPoint{};
            // pos
            newPoint.point[0] = p.x;
            newPoint.point[1] = p.y;
            newPoint.point[2] = p.z;
            // uv0
            newPoint.uv0[0] = p.u;
            newPoint.uv0[1] = p.v;
            // uv1
            newPoint.uv1[0] = p.lmU;
            newPoint.uv1[1] = p.lmV;

            // normal
            newPoint.nornmal[0] = p.nx;
            newPoint.nornmal[1] = p.ny;
            newPoint.nornmal[2] = p.nz;

            // convert poly3d point to SPolyPoint
            newPoly.AddPoint(newPoint);
        }
        m_polygons.push_back(newPoly);
        // update header info
        m_header.numFaces = static_cast<uint32_t>(m_polygons.size());
    }

    uint32_t MeshFile::AddDiffuseOnlyMaterial(const std::string& matkey, const std::string& diffuseName)
    {
        for (size_t i = 0; i < m_materials.size(); i++)
        {
            if (m_materials.at(i).GetMaterialKey() == matkey)
            {
                return static_cast<uint32_t>(i); // already loaded
            }
        }
        LogicalMaterial newMat;
        newMat.SetKey(matkey);
        newMat.AddTexture("diffuse", diffuseName);
        m_materials.push_back(newMat);

        // update header info
        m_header.numMaterials = static_cast<uint32_t>(m_materials.size());
        return m_header.numMaterials - 1;
    }

    bool MeshFile::WriteToFile(const std::string& filename)
    {
        using namespace mesh;
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp)
        {
            return false;
        }

        if (!ValidateData())
        {
            return false;
        }

        // header
        fwrite(&m_header, sizeof(SMeshHeader), 1, fp);
        // materials
        for (LogicalMaterial& mat : m_materials)
        {
            fwrite((mesh::SMaterialHeader*)mat.GetHeaderPtr(), sizeof(mesh::SMaterialHeader), 1, fp);

            for (mesh::STextureKey& tex : mat.GetTextureKeys())
            {
                fwrite((mesh::STextureKey*)&tex, sizeof(mesh::STextureKey), 1, fp);
            }
        }

        // polys
        for (LogicalPolygon& poly : m_polygons)
        {
            fwrite((mesh::SPolyHeader*)poly.GetHeaderPtr(), sizeof(mesh::SPolyHeader), 1, fp);
            for (mesh::SPolyPoint& point : poly.GetPoints())
            {
                fwrite((mesh::SPolyPoint*)&point, sizeof(mesh::SPolyPoint), 1, fp);
            }
        }

        // lightmaps
        for (LogicalLightmap& lm : m_lightmaps)
        {
            mesh::SLightmap* lightmap = lm.GetLightmapData();

            unsigned long compressedLen;
            unsigned char* compressedBuffer = CreateCompressedBuffer(
                    lightmap->data,
                    lm.GetHeaderPtr()->dataSize,
                    &compressedLen);

            // write header with compressed info
            lm.GetHeaderPtr()->compressedDataSize = compressedLen;
            fwrite((mesh::SLightmapHeader*)lm.GetHeaderPtr(), sizeof(mesh::SLightmapHeader), 1, fp);

            fwrite(compressedBuffer, compressedLen, 1, fp);

            delete[] compressedBuffer;
        }

        for (mesh::SLight& light : m_lights)
        {
            fwrite((mesh::SLight*)&light, sizeof(mesh::SLight), 1, fp);
        }

        fflush(fp);
        fclose(fp);
        return true;
    }

    bool MeshFile::ValidateData()
    {
        bool valid = true;

        // header
        if (m_header.numMaterials != m_materials.size())
        {
            Log("m_header.numMaterials %d != m_materials.size() %d\n",
                    m_header.numMaterials,
                    m_materials.size());
            valid = false;
        }
        if (m_header.numFaces != m_polygons.size())
        {
            Log("m_header.numFaces %d != m_polygons.size() %d\n",
                    m_header.numFaces,
                    m_polygons.size());
            valid = false;
        }

        // materials
        for (LogicalMaterial& mat : m_materials)
        {
            if (mat.GetTextureKeys().size() != mat.GetHeaderPtr()->numTextureKeys)
            {
                Log("mat.textureKeys.size() %d != mat.materialHeader.numTextureKeys %d\n",
                        mat.GetTextureKeys().size(),
                        mat.GetHeaderPtr()->numTextureKeys);
                valid = false;
            }
        }

        // polygons
        for (LogicalPolygon& poly : m_polygons)
        {
            if (poly.GetPoints().size() != poly.GetHeaderPtr()->numPoints)
            {
                Log("poly.points.size() %d != poly.polyHeader.numPoints %d !!\n",
                        poly.GetPoints().size(),
                        poly.GetHeaderPtr()->numPoints);
                valid = false;
            }
        }
        return valid;
    }

    void MeshFile::Reset()
    {
        memset(&m_header, 0, sizeof(mesh::SMeshHeader));
        m_materials.clear();
        m_polygons.clear();
    }

    void MeshFile::GetAsPolyList(std::vector<poly3d>& polyListOut)
    {
        for (LogicalPolygon& poly : m_polygons)
        {
            poly3d newPoly;

            uint16_t matIndex = poly.GetHeaderPtr()->matIndex;
            if (m_materials.size() > matIndex)
            {
                newPoly.SetMaterialKey(m_materials.at(matIndex).GetMaterialKey());
            }

            newPoly.SetLightmapDataIndex(poly.GetLightmapDataIndex());

            for (const mesh::SPolyPoint& spoint : poly.GetPoints())
            {
                rade::vector3 p(spoint.point);
                p.nx = spoint.nornmal[0];
                p.ny = spoint.nornmal[1];
                p.nz = spoint.nornmal[2];
                p.u = spoint.uv0[0];
                p.v = spoint.uv0[1];
                p.lmU = spoint.uv1[0];
                p.lmV = spoint.uv1[1];
                newPoly.AddPoint(p);
            }
            newPoly.CalcNormal();
            polyListOut.push_back(newPoly);
        }
    }

    void MeshFile::AddLightmapData(
            const uint16_t width,
            const uint16_t height,
            unsigned char* dataPtr,
            const unsigned int dataSize)
    {
        LogicalLightmap lightmap;
        lightmap.AddLightmapData(width, height, dataPtr, dataSize);

        m_lightmaps.push_back(lightmap);
        m_header.numLightmaps++;
    }

    void MeshFile::GetLightMaps(std::vector<CLightmapImg>& lmaps)
    {
        for (LogicalLightmap& lm : m_lightmaps)
        {
            CLightmapImg newLM;
            newLM.m_width = lm.GetHeaderPtr()->width;
            newLM.m_height = lm.GetHeaderPtr()->height;
            newLM.Allocate(newLM.m_width, newLM.m_height);
            int bufferSize = newLM.m_width * newLM.m_height * 4;
            memcpy(newLM.m_data, lm.GetLightmapData()->data, bufferSize);
            lmaps.push_back(newLM);
        }
    }

    void MeshFile::AddLight(const Light& light)
    {
        mesh::SLight newLight;
        memcpy(newLight.name, light.name.c_str(), std::max((int)light.name.length(), mesh::MATERIAL_NAME_LEN));
        light.pos.ToFloat3(newLight.pos);
        light.orientation.ToFloat3(newLight.dir);
        newLight.color[0] = light.color[0];
        newLight.color[1] = light.color[1];
        newLight.color[2] = light.color[2];
        newLight.radius = light.radius;
        newLight.brightness = light.brightness;
        m_lights.push_back(newLight);
        m_header.numLights = static_cast<uint32_t>(m_lights.size());
    }
};
