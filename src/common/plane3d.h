#pragma once

#include <vector>
#include "point3d.h"

class CPoly3D;

class CPlane3D
{
public:

    CPlane3D()
    {

    }

    // from 3 points
    CPlane3D(CPoint3D& p1, CPoint3D& p2, CPoint3D& p3);

    CPlane3D(const CPoly3D& poly);

    CPlane3D(CPoly3D* poly);

    CPlane3D(const float a, const float b, const float c, const float d);

    ~CPlane3D()
    {
    }

    enum EPlaneAxis
    {
        EPlaneAxis_YZ,
        EPlaneAxis_XZ,
        EPlaneAxis_XY
    };

    void InitFromPoints(const CPoint3D& p1, const CPoint3D& p2, const CPoint3D& p3);

    void InitFromPlane(float a, float b, float c, float d, bool normalize);

    CPoint3D GetNormal() const
    {
        return m_normal;
    }

    float GetDistance() const
    {
        return m_dist;
    }

    void Normalize();

    RMATH::ESide ClassifyPoint(const CPoint3D& point) const;

    RMATH::ESide ClassifyPoint(const CPoint3D& point, float sphere_radius) const;

    // returns closest axial plane (based on normal)
    EPlaneAxis GetPlaneAxis() const;

    float Distance(CPoint3D& point)
    {
        CPoint3D diff = point - m_pointOnPlane;
        return std::fabs(diff.Dot(m_normal));
    }

    // intersection
    bool GetRayIntersect(const CPoint3D& p1, const CPoint3D& p2, CPoint3D* intersect) const;

    bool GetLineSegmentIntersect(CPoint3D& p1, CPoint3D& p2, CPoint3D& intersect, bool recalcUV = true) const;

    bool Get3PlaneIntersection(CPlane3D& plane1, CPlane3D& plane2, CPoint3D* p) const;

    static void GetClosestAxialPlane(const CPoint3D& pNormal, CPoint3D* xv, CPoint3D* yv);

private:
    CPoint3D m_pointOnPlane;
    CPoint3D m_normal;
    float m_dist = 0.0f;
};