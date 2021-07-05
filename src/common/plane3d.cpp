#include "plane3d.h"
#include "polygon3d.h"

CPlane3D::CPlane3D(CPoint3D& p1, CPoint3D& p2, CPoint3D& p3)
{
    InitFromPoints(p1, p2, p3);
}

CPlane3D::CPlane3D(const CPoly3D& poly)
{
    InitFromPoints(poly.GetPoint(0),
            poly.GetPoint(1),
            poly.GetPoint(2));
}

CPlane3D::CPlane3D(CPoly3D* poly)
{
    InitFromPoints(poly->GetPoint(0),
            poly->GetPoint(1),
            poly->GetPoint(2));
}


CPlane3D::CPlane3D(const float a, const float b, const float c, const float d)
{
    InitFromPlane(a, b, c, d, false);
}

void CPlane3D::InitFromPlane(const float a, const float b, const float c, const float d, bool normalize)
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

void CPlane3D::Normalize()
{
    float distance = sqrtf(m_normal.x * m_normal.x + m_normal.y * m_normal.y + m_normal.z * m_normal.z);
    m_normal.x = m_normal.x / distance;
    m_normal.y = m_normal.y / distance;
    m_normal.z = m_normal.z / distance;
    m_dist = m_dist / distance;
}

void CPlane3D::InitFromPoints(const CPoint3D& p1, const CPoint3D& p2, const CPoint3D& p3)
{
    // calc normal
    //CPoint3D veca = p1 - p2;
    //CPoint3D vecb = p3 - p2;
    CPoint3D veca = p2 - p1;
    CPoint3D vecb = p3 - p1;
    m_normal = veca.CrossProduct(vecb);
    m_normal.Normalize();
    // calc distance
    m_dist = -m_normal.Dot(p1);
    Normalize();
}

RMATH::ESide CPlane3D::ClassifyPoint(const CPoint3D& point) const
{
    float p = m_normal.Dot(point) + m_dist;
    if (p < -RMATH::cEpsilon)
    {
        return RMATH::ESide_BACK;
    }
    else
    {
        if (p > RMATH::cEpsilon)
        {
            return RMATH::ESide_FRONT;
        }
    }
    return RMATH::ESide_ON;
}

RMATH::ESide CPlane3D::ClassifyPoint(const CPoint3D& point, float sphere_radius) const
{
    float p = m_normal.Dot(point) + m_dist + sphere_radius;
    if (p < 0)
    {
        return RMATH::ESide_BACK;
    }
    else
    {
        return RMATH::ESide_FRONT;
    }
}

// returns closest axial plane (based on normal)
CPlane3D::EPlaneAxis CPlane3D::GetPlaneAxis() const
{
    float max;
    EPlaneAxis plane;
    max = fabs(m_normal.x);
    plane = EPlaneAxis_YZ;

    if (fabs(m_normal.y) > max)
    {
        max = fabs(m_normal.y);
        plane = EPlaneAxis_XZ;
    }

    if (fabs(m_normal.z) > max)
    {
        //max = fabs(m_normal.z);
        plane = EPlaneAxis_XY;
    }
    return plane;
}

bool CPlane3D::GetRayIntersect(const CPoint3D& p1, const CPoint3D& p2, CPoint3D* intersect) const
{
    CPoint3D ray = p2 - p1;
    ray.Normalize();
    float dot_norm_ray = m_normal.Dot(ray);

    // early out if ray is opposite direction
    if (dot_norm_ray <= RMATH::cEpsilon)
        return false;

    float t = -(m_normal.Dot(p1) + m_dist) / dot_norm_ray;
    intersect->Set(p1.x + (ray.x * t),
            p1.y + (ray.y * t),
            p1.z + (ray.z * t));
    return true;
}

bool CPlane3D::Get3PlaneIntersection(CPlane3D& plane1, CPlane3D& plane2, CPoint3D* p) const
{
    CPoint3D n1 = GetNormal();
    CPoint3D n2 = plane1.GetNormal();
    CPoint3D n3 = plane2.GetNormal();

    float d1 = GetDistance();
    float d2 = plane1.GetDistance();
    float d3 = plane2.GetDistance();

    float denom = n1.Dot(n2.CrossProduct(n3));
    if (fabs(denom) < RMATH::cEpsilon)
        return false;

    CPoint3D t1 = n2.CrossProduct(n3);
    t1.Scale(-d1);

    CPoint3D t2 = n3.CrossProduct(n1);
    t2.Scale(-d2);

    CPoint3D t3 = n1.CrossProduct(n2);
    t3.Scale(-d3);

    *p = t1 + t2 + t3;
    p->Divide((float)denom);

    return true;
}

bool CPlane3D::GetLineSegmentIntersect(CPoint3D& p1, CPoint3D& p2, CPoint3D& intersect, bool recalcUV) const
{
    CPoint3D ray = p2 - p1;
    ray.Normalize();
    float dot_norm_ray = m_normal.Dot(ray);

    if (fabs(dot_norm_ray) <= RMATH::cEpsilon)
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
        if (fabs(origDist) >= RMATH::cEpsilon)
        {
            float newDist = p1.Distance(intersect); // get distance fromt point1 to newpoint
            float d = newDist / origDist; // now as percentage
            intersect.u = p1.u + ((p2.u - p1.u) * d); // move u and v along same percentage?
            intersect.v = p1.v + ((p2.v - p1.v) * d);
        }
    }
    return true;
}

void CPlane3D::GetClosestAxialPlane(const CPoint3D& pNormal, CPoint3D* xv, CPoint3D* yv)
{
    const float baseaxis[18][3] =
            {
                    { 0,  1,  0 },
                    { 1,  0,  0 },
                    { 0,  0,  1 },    // floor
                    { 0,  -1, 0 },
                    { 1,  0,  0 },
                    { 0,  0,  1 },    // ceiling
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
        CPoint3D axis(baseaxis[i * 3][0], baseaxis[i * 3][1], baseaxis[i * 3][2]);
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

