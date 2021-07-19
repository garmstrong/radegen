#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "point3d.h"
#include "transform.h"
#include "plane3d.h"
#include "frustrum.h"

namespace rade
{
    class Camera
    {
    public:
        Camera();

        // perspective projection transformation matrix
        glm::mat4 GetProjection() const;

        // translation and rotation matrix of the camera
        // Same as the GetMatrix but does not include the projection
        glm::mat4 GetView() const;

        // full matrix view and the projection
        glm::mat4 GetMatrix();

        void SetOrtho(bool ortho)
        {
            m_isOrtho = ortho;
        }

        // vertical viewing angle of the camera, in degrees.
        float GetFieldOfView() const;

        void SetFieldOfView(float fieldOfView);

        // closest visible distance from the camera
        float GetNearPlane() const;

        // the farthest visible distance from the camera.
        float GetFarPlane() const;

        // near and far plane distances
        void SetNearAndFarPlanes(float nearPlane, float farPlane);

        // Width divided by the Height of the screen/window/viewport
        float GetViewportAspectRatio() const;

        void SetViewportAspectRatio(float viewportAspectRatio);

        void SetViewport(float width, float height)
        {
            m_videoWidth = width;
            m_videoHeight = height;
            m_viewportAspectRatio = m_videoWidth / m_videoHeight;
        }

        rade::transform& GetTransform()
        {
            return m_transform;
        }

        bool IsInView(const rade::vector3& point);

    private:
        rade::transform m_transform;
        float m_fieldOfView;
        float m_nearPlane;
        float m_farPlane;
        float m_viewportAspectRatio;
        float m_videoWidth = 0.0f;
        float m_videoHeight = 0.0f;
        Frustrum m_frustrum;
        bool m_isOrtho = false;
    };
};
