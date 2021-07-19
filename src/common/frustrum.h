#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "point3d.h"
#include "transform.h"
#include "plane3d.h"

namespace rade
{
    class Frustrum
    {
    public:
        void UpdateFrustrumPlanes(const glm::mat4& mat);

        bool IsInsideFrustrum(const rade::vector3& point);

    private:
        rade::plane3d m_plane_left      {0.0f, 0.0f, 0.0f, 0.0f};
        rade::plane3d m_plane_right     {0.0f, 0.0f, 0.0f, 0.0f};
        rade::plane3d m_plane_bottom    {0.0f, 0.0f, 0.0f, 0.0f};
        rade::plane3d m_plane_top       {0.0f, 0.0f, 0.0f, 0.0f};
        rade::plane3d m_plane_near      {0.0f, 0.0f, 0.0f, 0.0f};
        rade::plane3d m_plane_far       {0.0f, 0.0f, 0.0f, 0.0f};
    };
}
