#pragma once

#include <string>
#include <vector>
#include <algorithm> // std::reverse
#include <point3d.h>

#include "rmath.h"

class CPlane3D;

class CPoly3D
{
public:

    CPoly3D();

    ~CPoly3D();

    void AddPoint(const CPoint3D& p);

    inline size_t NumPoints() const
    {
        return m_points.size();
    }

    CPoint3D GetPoint(unsigned int index) const
    {
        return m_points[index];
    }

    CPoint3D& GetPointRef(unsigned int index)
    {
        return m_points[index];
    }

    const std::vector<CPoint3D>& GetPointListRefConst() const
    {
        return m_points;
    }

    std::vector<CPoint3D>& GetPointListRef()
    {
        return m_points;
    }

    void SetPoint(unsigned int index, CPoint3D& newPoint)
    {
        m_points[index] = newPoint;
    }

    inline void ClearPoints()
    {
        m_points.clear();
    }

    void CalcNormal()
    {
        //CPoint3D veca = m_points[1] - m_points[2];
        //CPoint3D vecb = m_points[1] - m_points[0];
        CPoint3D veca = m_points[1] - m_points[0];
        CPoint3D vecb = m_points[2] - m_points[0];
        CPoint3D cross = veca.CrossProduct(vecb);
        cross.Normalize();
        m_normal.Set(cross);
    }

    inline void SetNormal(const CPoint3D& normal)
    {
        m_normal = normal;
    }

    double GetDistance() const
    {
        return m_distance;
    }

    void SetDistance(double distance)
    {
        m_distance = distance;
    }

    CPoint3D GetCenter() const;

    int PointInPoly(const CPoint3D& p) const;

    void SortWindingOrder();

    const CPoint3D& GetNormal() const
    {
        return m_normal;
    }

    std::vector<CPoly3D> ToTriangles() const;

    RMATH::ESide Split(const CPlane3D& plane, CPoly3D& front, CPoly3D& back) const;

    void TextureFromPlane(uint16_t texWidth, uint16_t texHeight,
            float scalex = 1.0f, float scaley = 1.0f,
            float shiftx = 0.0f, float shifty = 0.0f);

    RMATH::ESide ClassifyPolygon(const CPoly3D& poly);

    std::string GetMaterialKey() const
    {
        return m_materialKey;
    }

    void SetMaterialKey(const std::string& materialKey)
    {
        m_materialKey = materialKey;
    }

    void SetLightmapDataIndex(uint32_t id)
    {
        m_lightmapDataIndex = id;
    }

    uint32_t GetLightmapDataIndex() const
    {
        return m_lightmapDataIndex;
    }

    void SetLightTexID(uint32_t id)
    {
        m_lightmapTextureID = id;
    }

    uint32_t GetLightTexID() const
    {
        return m_lightmapTextureID;
    }

private:
    std::vector<CPoint3D> m_points;
    CPoint3D m_normal;
    double m_distance = 0;
    unsigned int m_flags = 0;

    // material key is path "base/door1" to texture (.png .jpg etc.. ) or a .json material file
    std::string m_materialKey;

    // id of material once loaded to display
    uint32_t m_materialIndex = 0;

    // special lightmap id index into data from file
    uint32_t m_lightmapDataIndex = 0;
    uint32_t m_lightmapTextureID = 0;
};
