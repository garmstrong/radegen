#include <limits>

#include "polygon3d.h"
#include "plane3d.h"
#include "osutils.h"

using namespace rade;

namespace rade
{
    CPoly3D::CPoly3D()
    = default;

    CPoly3D::~CPoly3D()
    = default;

    // sort verts in order from small angles from center to big
    void CPoly3D::SortWindingOrder()
    {
        rade::vector3 center = GetCenter();
        // order the points (may still be backwards winding)
        for (size_t i = 0; i < NumPoints(); i++)
        {
            float largestAngle = std::numeric_limits<float>::max();
            size_t largestIndex = -1;

            rade::vector3 vec1 = center - m_points.at(i);
            vec1.Normalize();

            for (size_t j = i + 1; j < m_points.size(); j++)
            {
                rade::vector3 vec2 = center - m_points.at(j);
                vec2.Normalize();
                float vAngle = vec1.Dot(vec2); // angle from center of poly to this vert

                // dot only works for angles 0 to 180
                // seen in "Ryan Samuel Gregg's" MAPViewer source
                rade::vector3 cross = vec1.CrossProduct(vec2);
                if (cross.Dot(m_normal) < 0.00f)
                {
                    vAngle += 2.0f;
                }

                if (vAngle > largestAngle)
                {
                    largestAngle = vAngle;
                    largestIndex = j;
                }
            }

            if (largestIndex != -1)
            {
                rade::vector3 tmp = m_points.at(i + 1);
                m_points.at(i + 1) = m_points.at(largestIndex);
                m_points.at(largestIndex) = tmp;
            }
        }
        // TODO: logic above needs to go backwards
        std::reverse(m_points.begin(), m_points.end());

        // verts are now sorted, re-calc normal for this potentially different winding
        CalcNormal();
    }

    void CPoly3D::AddPoint(const rade::vector3& p)
    {
        m_points.push_back(p);
    }

    int CPoly3D::PointInPoly(const rade::vector3& p) const
    {
        double angles = 0.0f;
        for (unsigned int i = 0; i < NumPoints(); i++)
        {
            rade::vector3 veca = GetPoint(i) - p;
            rade::vector3 vecb;
            if (i == NumPoints() - 1)
            {
                vecb = GetPoint(0) - p;
            }
            else
            {
                vecb = GetPoint(i + 1) - p;
            }
            veca.Normalize();
            vecb.Normalize();
            angles += acosf(veca.Dot(vecb));
        }

        if (fabs((angles - math::c2Pi)) < math::cEpsilonLarger)
            return 1;

        return 0;
    }

    rade::vector3 CPoly3D::GetCenter() const
    {
        rade::vector3 center;
        for (const auto& m_point : m_points)
        {
            center = center + m_point;
        }
        center.Divide((float)m_points.size());
        return center;
    }

    std::vector<CPoly3D> CPoly3D::ToTriangles() const
    {
        //TODO: simple triangle fan might cause problems, look into ear clipping
        std::vector<CPoly3D> triangles;
        if (NumPoints() == 3)
        {
            triangles.push_back(*this);
        }
        else
        {
            for (unsigned int i = 0; i < NumPoints() - 2; i++)
            {
                CPoly3D triangle = *this;
                triangle.ClearPoints();
                triangle.AddPoint(GetPoint(0));
                triangle.AddPoint(GetPoint(i + 1));
                triangle.AddPoint(GetPoint(i + 2));
                triangles.push_back(triangle);
            }
        }
        return triangles;
    }

    math::ESide CPoly3D::Split(const plane3d& plane, CPoly3D& front, CPoly3D& back) const
    {
        bool hasFront = false;
        bool hasBack = false;

        front = *this;
        back = *this;

        front.ClearPoints();
        back.ClearPoints();

        math::ESide polySide;
        for (unsigned int i = 0; i < NumPoints(); i++)
        {
            rade::vector3 p0 = GetPoint(i);
            int side0 = plane.ClassifyPoint(p0);

            rade::vector3 p1;
            if (i == NumPoints() - 1) // p1 will be last point. loop back to 0
            {
                p1 = GetPoint(0);
            }
            else
            {
                p1 = GetPoint(i + 1);
            }

            int side1 = plane.ClassifyPoint(p1);

            switch (side0)
            {
            case math::ESide_ON:
                back.AddPoint(p0);
                front.AddPoint(p0);
                break;

            case math::ESide_FRONT:
                hasFront = true;
                front.AddPoint(p0);
                if (side1 == math::ESide_BACK)
                {
                    // get intersect, add it to front and back
                    rade::vector3 newPoint;
                    plane.GetLineSegmentIntersect(p1, p0, newPoint);
                    front.AddPoint(newPoint);
                    back.AddPoint(newPoint);
                }
                break;

            case math::ESide_BACK:
                hasBack = true;
                back.AddPoint(p0);
                if (side1 == math::ESide_FRONT)
                {
                    // get intersect, add it to front and back
                    rade::vector3 newPoint;
                    plane.GetLineSegmentIntersect(p1, p0, newPoint);
                    front.AddPoint(newPoint);
                    back.AddPoint(newPoint);
                }
                break;

            default:
                rade::Abort("Unhandled plane.ClassifyPoint %d\n", side0);
            }
        }

        if (hasFront && hasBack) // spanning
        {
            polySide = math::ESide_SPAN;
            // TODO: does the splitting require poly.SortWinding ?
            if (front.NumPoints() == 0 || back.NumPoints() == 0)
            {
                rade::Assert("%s created spanned polygon split but %d points on back, %d points on front\n",
                        __func__, back.NumPoints(), front.NumPoints());
            }
        }
        else if (hasFront && !hasBack)
        {
            // all on front
            polySide = math::ESide_FRONT;
        }
        else if (!hasFront && hasBack)
        {
            // all on back
            polySide = math::ESide_BACK;
        }
        else
        {
            polySide = math::ESide_ON;
        }
        return polySide;
    }

    void CPoly3D::TextureFromPlane(uint16_t texWidth, uint16_t texHeight, const float scalex, const float scaley,
            const float shiftx, const float shifty)
    {
        for (size_t i = 0; i < NumPoints(); i++)
        {
            rade::vector3* point = &m_points.at(i);
            // calc tex co-ords
            float W = 1.0f / static_cast<float>(texWidth);
            float H = 1.0f / static_cast<float>(texHeight);
            rade::vector3 uAxis, vAxis;
            plane3d::GetClosestAxialPlane(GetNormal(), &uAxis, &vAxis);

            point->u = (point->Dot(uAxis) * W / scalex) + (shiftx * W);
            point->v = (point->Dot(vAxis) * H / scaley) + (shifty * H);

            // TODO: rotate
        }
    }

    math::ESide CPoly3D::ClassifyPolygon(const CPoly3D& poly)
    {
        int frontCount = 0;
        int backCount = 0;
        int onCount = 0;
        size_t vertCount = NumPoints();

        SortWindingOrder();
        //plane3d plane = plane3d(*this);
        plane3d plane = GetPlane();

        for (unsigned int i = 0; i < vertCount; i++)
        {
            rade::vector3 p0 = poly.GetPoint(i);
            int side0 = plane.ClassifyPoint(p0);

            switch (side0)
            {
            case math::ESide_ON:
                // add to front and back
                onCount++;
                break;

            case math::ESide_FRONT:
                frontCount++;
                break;

            case math::ESide_BACK:
                backCount++;
                break;

            default:
                break;
            }
        }

        if (vertCount == frontCount)
        {
            return math::ESide_FRONT;
        }
        else if (vertCount == backCount)
        {
            return math::ESide_BACK;
        }
        else if (vertCount == onCount)
        {
            return math::ESide_ON;
        }
        return math::ESide_SPAN;
    }

    plane3d CPoly3D::GetPlane() const
    {
        plane3d plane(GetPoint(0), GetPoint(1), GetPoint(2));
        return plane;
    }
};
