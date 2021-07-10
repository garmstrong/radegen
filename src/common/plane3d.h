#pragma once

#include <vector>
#include "point3d.h"

class CPoly3D;

namespace rade
{
    class plane3d
    {
    public:

        // from 3 points
        plane3d(const vector3& p1, const vector3& p2, const vector3& p3);

        plane3d(float a, float b, float c, float d);

        enum EPlaneAxis
        {
            EPlaneAxis_YZ,
            EPlaneAxis_XZ,
            EPlaneAxis_XY
        };

        void InitFromPoints(const vector3& p1, const vector3& p2, const vector3& p3);

        void InitFromPlane(float a, float b, float c, float d, bool normalize);

        rade::vector3 GetNormal() const
        {
            return m_normal;
        }

        float GetDistance() const
        {
            return m_dist;
        }

        void Normalize();

        rade::math::ESide ClassifyPoint(const vector3& point) const;

        rade::math::ESide ClassifyPoint(const vector3& point, float sphere_radius) const;

        // returns closest axial plane (based on normal)
        EPlaneAxis GetPlaneAxis() const;

        float Distance(vector3& point)
        {
            vector3 diff = point - m_pointOnPlane;
            return std::fabs(diff.Dot(m_normal));
        }

        // intersection
        bool GetRayIntersect(const vector3& p1, const vector3& p2, vector3* intersect) const;

        bool GetLineSegmentIntersect(vector3& p1, vector3& p2, vector3& intersect,
                bool recalcUV = true) const;

        bool Get3PlaneIntersection(plane3d& plane1, plane3d& plane2, vector3* p) const;

        static void GetClosestAxialPlane(const vector3& pNormal, vector3* xv, vector3* yv);

    private:
        vector3 m_pointOnPlane;
        vector3 m_normal;
        float m_dist = 0.0f;
    };

};
