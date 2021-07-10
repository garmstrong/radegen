#pragma once

#include <string>
#include <vector>
#include <algorithm> // std::reverse
#include <point3d.h>

#include "rmath.h"

namespace rade
{
    class plane3d;

    class CPoly3D
    {
    public:

        CPoly3D();

        ~CPoly3D();

        void AddPoint(const vector3& p);

        inline size_t NumPoints() const
        {
            return m_points.size();
        }

        vector3 GetPoint(unsigned int index) const
        {
            return m_points[index];
        }

        vector3& GetPointRef(unsigned int index)
        {
            return m_points[index];
        }

        const std::vector<rade::vector3>& GetPointListRefConst() const
        {
            return m_points;
        }

        std::vector<rade::vector3>& GetPointListRef()
        {
            return m_points;
        }

        void SetPoint(unsigned int index, rade::vector3& newPoint)
        {
            m_points[index] = newPoint;
        }

        inline void ClearPoints()
        {
            m_points.clear();
        }

        void CalcNormal()
        {
            //vector3 veca = m_points[1] - m_points[2];
            //vector3 vecb = m_points[1] - m_points[0];
            rade::vector3 veca = m_points[1] - m_points[0];
            rade::vector3 vecb = m_points[2] - m_points[0];
            rade::vector3 cross = veca.CrossProduct(vecb);
            cross.Normalize();
            m_normal.Set(cross);
        }

        inline void SetNormal(const rade::vector3& normal)
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

        rade::vector3 GetCenter() const;

        int PointInPoly(const rade::vector3& p) const;

        void SortWindingOrder();

        const rade::vector3& GetNormal() const
        {
            return m_normal;
        }

        std::vector<CPoly3D> ToTriangles() const;

        rade::math::ESide Split(const rade::plane3d& plane, CPoly3D& front, CPoly3D& back) const;

        void TextureFromPlane(uint16_t texWidth, uint16_t texHeight,
                float scalex = 1.0f, float scaley = 1.0f,
                float shiftx = 0.0f, float shifty = 0.0f);

        rade::math::ESide ClassifyPolygon(const CPoly3D& poly);

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

        plane3d GetPlane() const;

    private:
        std::vector<rade::vector3> m_points;
        rade::vector3 m_normal;
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
};
