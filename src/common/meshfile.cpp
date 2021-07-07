#include <memory.h>
#include <cstdio>

#include "meshfile.h"
#include "miniz.h"

CMeshFile::CMeshFile()
{
    memset(&m_header, 0, sizeof(NMeshFile::SMeshHeader));
}

CMeshFile::~CMeshFile()
{
    FreeAll();
}

unsigned char* CMeshFile::CreateCompressedBuffer(
        unsigned char* pData,
        unsigned long nDataSize,
        unsigned long* nCompressedDataSize)
{
    *nCompressedDataSize = nDataSize;
    unsigned char* pCompressedData = new unsigned char[*nCompressedDataSize];

    //int nResult = compress2(pCompressedData, nCompressedDataSize, pData, nDataSize, 9);
    int nResult = compress(pCompressedData, nCompressedDataSize, pData, nDataSize);
    if (nResult != Z_OK)
    {
        return nullptr;
    }
    return pCompressedData;
}

unsigned char* CMeshFile::CreateUncompressedBuffer(unsigned char* pCompressedData,
        unsigned long nCompressedDataSize,
        unsigned long nOriginalDataSize)
{

    unsigned char* pUncompressedData = new unsigned char[nOriginalDataSize];
    int nResult = uncompress(pUncompressedData, &nOriginalDataSize, pCompressedData, nCompressedDataSize);
    if (nResult != Z_OK)
    {
        return nullptr;
    }
    return pUncompressedData;
}

void CMeshFile::FreeAll()
{
    DeleteLightmapData();
}

void CMeshFile::DeleteLightmapData()
{
    for (int i = 0; i < m_lightmaps.size(); i++)
    {
        unsigned char* dataPtr = m_lightmaps.at(i).GetLightmapData()->data;
        if (dataPtr)
        {
            delete[] dataPtr;
            m_lightmaps.at(i).GetLightmapData()->data = nullptr;
        }
    }
    m_lightmaps.clear();

    m_header.numLightmaps = 0;
}

bool CMeshFile::LoadFromFile(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
        return false;
    }

    Reset();

    fread((NMeshFile::SMeshHeader*)&m_header, sizeof(NMeshFile::SMeshHeader), 1, fp);

    // materials
    for (int i = 0; i < m_header.numMaterials; i++)
    {
        CLogicalMaterial mat;

        NMeshFile::SMaterialHeader matHeader{};
        fread((NMeshFile::SMaterialHeader*)&matHeader, sizeof(NMeshFile::SMaterialHeader), 1, fp);
        mat.SetKey(matHeader.materialName);

        for (int j = 0; j < matHeader.numTextureKeys; j++)
        {
            NMeshFile::STextureKey tex{};
            fread((NMeshFile::STextureKey*)&tex, sizeof(NMeshFile::STextureKey), 1, fp);
            mat.AddTexture(tex.keyName, tex.fileName);
        }
        m_materials.push_back(mat);
    }

    // polys
    for (int i = 0; i < m_header.numFaces; i++)
    {
        CLogicalPolygon poly;

        NMeshFile::SPolyHeader polyHeader{};
        fread((NMeshFile::SPolyHeader*)&polyHeader, sizeof(NMeshFile::SPolyHeader), 1, fp);
        poly.SetMaterialIndex(polyHeader.matIndex);
        poly.SetLightmapDataIndex(polyHeader.lmIndex);

        for (int j = 0; j < polyHeader.numPoints; j++)
        {
            NMeshFile::SPolyPoint point;
            fread((NMeshFile::SPolyPoint*)&point, sizeof(NMeshFile::SPolyPoint), 1, fp);
            poly.AddPoint(point);
        }
        m_polygons.push_back(poly);
    }

    // lightmaps
    for (int i = 0; i < m_header.numLightmaps; i++)
    {
        CLogicalLightmap newLightmap;

        size_t lmSize = sizeof(NMeshFile::SLightmapHeader);

        NMeshFile::SLightmapHeader lmHeader;
        fread((NMeshFile::SLightmapHeader*)&lmHeader, sizeof(NMeshFile::SLightmapHeader), 1, fp);

        unsigned char* compressedBuffer = new unsigned char[lmHeader.compressedDataSize];
        fread((unsigned char*)compressedBuffer, lmHeader.compressedDataSize, 1, fp);

        NMeshFile::SLightmap lightmap;
        lightmap.data = CreateUncompressedBuffer(compressedBuffer, lmHeader.compressedDataSize, lmHeader.dataSize);
        delete[] compressedBuffer;

        newLightmap.AddLightmapData(lmHeader.width, lmHeader.height, lightmap.data, lmHeader.dataSize);
        delete[] lightmap.data;

        m_lightmaps.push_back(newLightmap);
    }

    for (int i = 0; i < m_header.numLights; i++)
    {
        NMeshFile::SLight light;
        fread((NMeshFile::SLight*)&light, sizeof(NMeshFile::SLight), 1, fp);
        m_lights.push_back(light);
    }

    ValidateData();

    fclose(fp);
    return true;
}

void CMeshFile::LoadFromPolyList(const std::vector<CPoly3D>& polylist)
{
    Reset();
    for (const CPoly3D& poly : polylist)
    {
        AddPoly(poly);
    }
    ValidateData();
}

void CMeshFile::AddPoly(const CPoly3D& poly)
{
    CLogicalPolygon newPoly{};

    // use key bor material name and diffuse tex
    uint16_t matIndex = AddDiffuseOnlyMaterial(poly.GetMaterialKey(), poly.GetMaterialKey());
    newPoly.SetMaterialIndex(matIndex);
    newPoly.SetLightmapDataIndex(poly.GetLightmapDataIndex());

    const std::vector<CPoint3D>& pointsRef = poly.GetPointListRefConst();
    for (const auto & p : pointsRef)
    {
        NMeshFile::SPolyPoint newPoint{};
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

        // convert CPoly3D point to SPolyPoint
        newPoly.AddPoint(newPoint);
    }

    m_polygons.push_back(newPoly);
    // update header info
    m_header.numFaces = m_polygons.size();
}

uint32_t CMeshFile::AddDiffuseOnlyMaterial(const std::string& matkey, const std::string& diffuseName)
{
    for (size_t i = 0; i < m_materials.size(); i++)
    {
        if (m_materials.at(i).GetMaterialKey() == matkey)
        {
            return i; // already loaded
        }
    }

    CLogicalMaterial newMat;
    newMat.SetKey(matkey);
    newMat.AddTexture("diffuse", diffuseName);
    m_materials.push_back(newMat);

    // update header info
    m_header.numMaterials = m_materials.size();
    return m_header.numMaterials - 1;
}

bool CMeshFile::WriteToFile(const std::string& filename)
{
    using namespace NMeshFile;
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
    for (CLogicalMaterial& mat : m_materials)
    {
        fwrite((NMeshFile::SMaterialHeader*)mat.GetHeaderPtr(), sizeof(NMeshFile::SMaterialHeader), 1, fp);

        for (NMeshFile::STextureKey& tex : mat.GetTextureKeys())
        {
            fwrite((NMeshFile::STextureKey*)&tex, sizeof(NMeshFile::STextureKey), 1, fp);
        }
    }

    // polys
    for (CLogicalPolygon& poly : m_polygons)
    {
        fwrite((NMeshFile::SPolyHeader*)poly.GetHeaderPtr(), sizeof(NMeshFile::SPolyHeader), 1, fp);
        for (NMeshFile::SPolyPoint& point : poly.GetPoints())
        {
            fwrite((NMeshFile::SPolyPoint*)&point, sizeof(NMeshFile::SPolyPoint), 1, fp);
        }
    }

    // lightmaps
    for (CLogicalLightmap& lm : m_lightmaps)
    {
        NMeshFile::SLightmap* lightmap = lm.GetLightmapData();

        unsigned long compressedLen;
        unsigned char* compressedBuffer = CreateCompressedBuffer(
                lightmap->data,
                lm.GetHeaderPtr()->dataSize,
                &compressedLen);

        // write header with compressed info
        lm.GetHeaderPtr()->compressedDataSize = compressedLen;
        fwrite((NMeshFile::SLightmapHeader*)lm.GetHeaderPtr(), sizeof(NMeshFile::SLightmapHeader), 1, fp);

        fwrite(compressedBuffer, compressedLen, 1, fp);

        delete[] compressedBuffer;
    }

    for (NMeshFile::SLight & light : m_lights)
    {
        fwrite((NMeshFile::SLight*)&light, sizeof(NMeshFile::SLight), 1, fp);
    }

    fflush(fp);
    fclose(fp);
    return true;
}

bool CMeshFile::ValidateData()
{
    bool valid = true;

    // header
    if (m_header.numMaterials != m_materials.size())
    {
        printf("m_header.numMaterials != m_materials.size()\n",
                m_header.numMaterials,
                m_materials.size());
        valid = false;
    }
    if (m_header.numFaces != m_polygons.size())
    {
        printf("m_header.numFaces %d != m_polygons.size() %d\n",
                m_header.numFaces,
                m_polygons.size());
        valid = false;
    }

    // materials
    for (CLogicalMaterial& mat : m_materials)
    {
        if (mat.GetTextureKeys().size() != mat.GetHeaderPtr()->numTextureKeys)
        {
            printf("mat.textureKeys.size() %d != mat.materialHeader.numTextureKeys %d\n",
                    mat.GetTextureKeys().size(),
                    mat.GetHeaderPtr()->numTextureKeys);
            valid = false;
        }
    }

    // polygons
    for (CLogicalPolygon& poly : m_polygons)
    {
        if (poly.GetPoints().size() != poly.GetHeaderPtr()->numPoints)
        {
            printf("poly.points.size() %d != poly.polyHeader.numPoints %d !!\n",
                    poly.GetPoints().size(),
                    poly.GetHeaderPtr()->numPoints);
            valid = false;
        }
    }
    return valid;
}

void CMeshFile::Reset()
{
    memset(&m_header, 0, sizeof(NMeshFile::SMeshHeader));
    m_materials.clear();
    m_polygons.clear();
}

void CMeshFile::GetAsPolyList(std::vector<CPoly3D>& polyListOut)
{
    for (CLogicalPolygon& poly : m_polygons)
    {
        CPoly3D newPoly;

        uint16_t matIndex = poly.GetHeaderPtr()->matIndex;
        if (m_materials.size() > matIndex)
        {
            newPoly.SetMaterialKey(m_materials.at(matIndex).GetMaterialKey());
        }

        newPoly.SetLightmapDataIndex(poly.GetLightmapDataIndex());

        const std::vector<NMeshFile::SPolyPoint>& points = poly.GetPoints();

        //for (auto point = points.rbegin(); point != points.rend(); ++point)
        for (const NMeshFile::SPolyPoint& spoint : points)
        {
            CPoint3D p(spoint.point);
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

void CMeshFile::AddLightmapData(
        const uint16_t width,
        const uint16_t height,
        unsigned char* dataPtr,
        const unsigned int dataSize)
{
    CLogicalLightmap lightmap;
    lightmap.AddLightmapData(width, height, dataPtr, dataSize);

    m_lightmaps.push_back(lightmap);
    m_header.numLightmaps++;
}

void CMeshFile::GetLightMaps(std::vector<CLightmapImg>& lmaps)
{
    for (CLogicalLightmap& lm : m_lightmaps)
    {
        CLightmapImg newLM;
        newLM.m_width = lm.GetHeaderPtr()->width;
        newLM.m_height = lm.GetHeaderPtr()->height;

        // = lm.GetLightmapData()->data;

        newLM.Allocate(newLM.m_width, newLM.m_height);

        int bufferSize = newLM.m_width * newLM.m_height * 4;
        memcpy(newLM.m_data, lm.GetLightmapData()->data, bufferSize);

        lmaps.push_back(newLM);
    }
}

void CMeshFile::AddLight(const CLight &light)
{
    NMeshFile::SLight newLight;
    memcpy(newLight.name, light.name.c_str(), std::max((int)light.name.length(), NMeshFile::MATERIAL_NAME_LEN));
    light.pos.ToFloat3(newLight.pos);
    light.orientation.ToFloat3(newLight.dir);
    newLight.color[0] = light.color[0];
    newLight.color[1] = light.color[1];
    newLight.color[2] = light.color[2];
    newLight.radius = light.radius;
    newLight.brightness = light.brightness;
    m_lights.push_back(newLight);
    m_header.numLights = m_lights.size();
}
