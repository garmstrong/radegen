#include "plane3d.h"

namespace rade
{
    plane3d::plane3d(const vector3& p1, const vector3& p2, const vector3& p3)
    {
        InitFromPoints(p1, p2, p3);
    }

    plane3d::plane3d(const float a, const float b, const float c, const float d)
    {
        InitFromPlane(a, b, c, d, false);
    }

    void plane3d::InitFromPlane(const float a, const float b, const float c, const float d, bool normalize)
    {
        m_normal.x = a;
        m_normal.y = b;
        m_normal.z = c;
        m_dist = d;

        if (normalize)
        {
            Normalize();
        }
    }

    void plane3d::Normalize()
    {
        float distance = sqrtf(m_normal.x * m_normal.x + m_normal.y * m_normal.y + m_normal.z * m_normal.z);
        m_normal.x = m_normal.x / distance;
        m_normal.y = m_normal.y / distance;
        m_normal.z = m_normal.z / distance;
        m_dist = m_dist / distance;
    }

    void plane3d::InitFromPoints(const rade::vector3& p1, const rade::vector3& p2, const rade::vector3& p3)
    {
        // calc normal
        //vector3 veca = p1 - p2;
        //vector3 vecb = p3 - p2;
        rade::vector3 veca = p2 - p1;
        rade::vector3 vecb = p3 - p1;
        m_normal = veca.CrossProduct(vecb);
        m_normal.Normalize();
        // calc distance
        m_dist = -m_normal.Dot(p1);
        Normalize();
    }

    rade::math::ESide plane3d::ClassifyPoint(const rade::vector3& point) const
    {
        float p = m_normal.Dot(point) + m_dist;
        if (p < -rade::math::cEpsilon)
        {
            return rade::math::ESide_BACK;
        }
        else
        {
            if (p > rade::math::cEpsilon)
            {
                return rade::math::ESide_FRONT;
            }
        }
        return rade::math::ESide_ON;
    }

    rade::math::ESide plane3d::ClassifyPoint(const rade::vector3& point, float sphere_radius) const
    {
        float p = m_normal.Dot(point) + m_dist + sphere_radius;
        if (p < 0)
        {
            return rade::math::ESide_BACK;
        }
        else
        {
            return rade::math::ESide_FRONT;
        }
    }

    // returns closest axial plane (based on normal)
    plane3d::EPlaneAxis plane3d::GetPlaneAxis() const
    {
        float max;
        EPlaneAxis plane;
        max = fabsf(m_normal.x);
        plane = EPlaneAxis_YZ;

        if (fabsf(m_normal.y) > max)
        {
            max = fabsf(m_normal.y);
            plane = EPlaneAxis_XZ;
        }

        if (fabsf(m_normal.z) > max)
        {
            //max = fabs(m_normal.z);
            plane = EPlaneAxis_XY;
        }
        return plane;
    }

    bool plane3d::GetRayIntersect(const rade::vector3& p1, const rade::vector3& p2, rade::vector3* intersect) const
    {
        rade::vector3 ray = p2 - p1;
        ray.Normalize();
        float dot_norm_ray = m_normal.Dot(ray);

        // early out if ray is opposite direction
        if (dot_norm_ray <= rade::math::cEpsilon)
            return false;

        float t = -(m_normal.Dot(p1) + m_dist) / dot_norm_ray;
        if( fabs(t) >rade::math::cEpsilonLarger )
        {
            intersect->Set(p1.x + (ray.x * t),
                    p1.y + (ray.y * t),
                    p1.z + (ray.z * t));
            return true;
        }
        else
        {
            return false;
        }
    }

    bool plane3d::GetRayIntersection(const rade::vector3& point, const rade::vector3& ray, rade::vector3* intersect) const
    {
        bool hit = false;

        float dot_norm_ray = m_normal.Dot(ray);

        // early out if ray is opposite direction
        if (dot_norm_ray <= rade::math::cEpsilon)
            return false;

        float t = -(m_normal.Dot(point) + m_dist) / dot_norm_ray;
        if( t>rade::math::cEpsilon)
        {
            hit = true;
            if (intersect)
            {
                intersect->Set(point.x + (ray.x * t),
                        point.y + (ray.y * t),
                        point.z + (ray.z * t));
            }
        }
        return hit;
    }

    bool plane3d::Get3PlaneIntersection(plane3d& plane1, plane3d& plane2, rade::vector3* p) const
    {
        rade::vector3 n1 = GetNormal();
        rade::vector3 n2 = plane1.GetNormal();
        rade::vector3 n3 = plane2.GetNormal();

        float d1 = GetDistance();
        float d2 = plane1.GetDistance();
        float d3 = plane2.GetDistance();

        float denom = n1.Dot(n2.CrossProduct(n3));
        if (fabsf(denom) < rade::math::cEpsilon)
            return false;

        rade::vector3 t1 = n2.CrossProduct(n3);
        t1.Scale(-d1);

        rade::vector3 t2 = n3.CrossProduct(n1);
        t2.Scale(-d2);

        rade::vector3 t3 = n1.CrossProduct(n2);
        t3.Scale(-d3);

        *p = t1 + t2 + t3;
        p->Divide((float)denom);

        return true;
    }

    bool plane3d::GetLineSegmentIntersect(rade::vector3& p1, rade::vector3& p2, rade::vector3& intersect,
            bool recalcUV) const
    {
        rade::vector3 ray = p2 - p1;
        ray.Normalize();
        float dot_norm_ray = m_normal.Dot(ray);

        if (fabsf(dot_norm_ray) <= rade::math::cEpsilon)
        {
            // no intersection
            return false;
        }

        float t = -(m_normal.Dot(p1) + m_dist) / dot_norm_ray;
        intersect.Set(p1.x + (ray.x * t),
                p1.y + (ray.y * t),
                p1.z + (ray.z * t));

        // calculate the interpolated UV coords for intersection point
        if (recalcUV)
        {
            float origDist = p1.Distance(p2); // get m_dist from point1 to point2
            if (fabsf(origDist) >= rade::math::cEpsilon)
            {
                float newDist = p1.Distance(intersect); // get distance fromt point1 to newpoint
                float d = newDist / origDist; // now as percentage
                intersect.u = p1.u + ((p2.u - p1.u) * d); // move u and v along same percentage?
                intersect.v = p1.v + ((p2.v - p1.v) * d);
            }
        }
        return true;
    }

    void plane3d::GetClosestAxialPlane(const rade::vector3& pNormal, rade::vector3* xv, rade::vector3* yv)
    {
        const float baseaxis[18][3] =
                {
                        { 0,  1,  0 },
                        { 1,  0,  0 },
                        { 0,  0,  1 },   // floor
                        { 0,  -1, 0 },
                        { 1,  0,  0 },
                        { 0,  0,  1 },   // ceiling
                        { 1,  0,  0 },
                        { 0,  0,  -1 },
                        { 0,  -1, 0 },   // west wall
                        { -1, 0,  0 },
                        { 0,  0,  -1 },
                        { 0,  -1, 0 },   // east wall
                        { 0,  0,  -1 },
                        { 1,  0,  0 },
                        { 0,  -1, 0 },   // south wall
                        { 0,  0,  1 },
                        { 1,  0,  0 },
                        { 0,  -1, 0 }    // north wall
                };

        int bestaxis;
        float dot, best;
        int i;

        best = 0;
        bestaxis = 0;

        for (i = 0; i < 6; i++)
        {
            rade::vector3 axis(baseaxis[i * 3][0], baseaxis[i * 3][1], baseaxis[i * 3][2]);
            dot = pNormal.Dot(axis);
            if (dot > best)
            {
                best = dot;
                bestaxis = i;
            }
        }
        xv->Set(baseaxis[bestaxis * 3 + 1][0], baseaxis[bestaxis * 3 + 1][1], baseaxis[bestaxis * 3 + 1][2]);
        yv->Set(baseaxis[bestaxis * 3 + 2][0], baseaxis[bestaxis * 3 + 2][1], baseaxis[bestaxis * 3 + 2][2]);
    }
};
