#include "frustrum.h"

namespace rade
{
    void Frustrum::UpdateFrustrumPlanes(const glm::mat4& mat)
    {
        // see Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix
        // http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
        // Gil Gribb & Klaus Hartmann

        float left[4];
        float right[4];
        float top[4];
        float bottom[4];
        float near[4];
        float far[4];

        for (int i = 4; i--;) left[i] = mat[i][3] + mat[i][0];
        for (int i = 4; i--;) right[i] = mat[i][3] - mat[i][0];
        for (int i = 4; i--;) bottom[i] = mat[i][3] + mat[i][1];
        for (int i = 4; i--;) top[i] = mat[i][3] - mat[i][1];
        for (int i = 4; i--;) near[i] = mat[i][3] + mat[i][2];
        for (int i = 4; i--;) far[i] = mat[i][3] - mat[i][2];

        bool normalise = true;
        m_plane_left.InitFromPlane(left[0], left[1], left[2], left[3], normalise);
        m_plane_right.InitFromPlane(right[0], right[1], right[2], right[3], normalise);
        m_plane_bottom.InitFromPlane(bottom[0], bottom[1], bottom[2], bottom[3], normalise);
        m_plane_top.InitFromPlane(top[0], top[1], top[2], top[3], normalise);
        m_plane_near.InitFromPlane(near[0], near[1], near[2], near[3], normalise);
        m_plane_far.InitFromPlane(far[0], far[1], far[2], far[3], normalise);
    }

    bool Frustrum::IsInsideFrustrum(const vector3& point)
    {
        using namespace rade::math;
        float radius = 5.0f;

        if (m_plane_left.ClassifyPoint(point, radius) == rade::math::ESide_BACK)
            return false;

        if (m_plane_right.ClassifyPoint(point, radius) == ESide_BACK)
            return false;

        if (m_plane_bottom.ClassifyPoint(point, radius) == ESide_BACK)
            return false;

        if (m_plane_top.ClassifyPoint(point, radius) == ESide_BACK)
            return false;

        if (m_plane_near.ClassifyPoint(point, radius) == ESide_BACK)
            return false;

        if (m_plane_far.ClassifyPoint(point, radius) == ESide_BACK)
            return false;

        return true;
    }
}
