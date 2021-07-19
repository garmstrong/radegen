#include "camera.h"
#include "osutils.h"

using namespace rade;

static const float MaxVerticalAngle = 88.0f; //must be less than 90 to avoid gimbal lock

namespace rade
{
    Camera::Camera() :
            m_fieldOfView(50.0f),
            m_nearPlane(0.01f),
            m_farPlane(100.0f),
            m_viewportAspectRatio(4.0f / 3.0f)
    {
    }

    glm::mat4 Camera::GetProjection() const
    {
        if (m_isOrtho)
        {
            return glm::ortho(
                    -(m_videoWidth / 2),    // left
                    (m_videoWidth / 2),     // right
                    -(m_videoHeight / 2),   // bottom
                    (m_videoHeight / 2),    // top
                    -1000.0f,
                    1000.0f);
        }
        else
        {
            return glm::perspective(glm::radians(m_fieldOfView), m_viewportAspectRatio, m_nearPlane, m_farPlane);
        }
    }

    glm::mat4 Camera::GetView() const
    {
        return m_transform.GetMatrix();
    }

    glm::mat4 Camera::GetMatrix()
    {
        return GetProjection() * m_transform.GetMatrix();
    }

    float Camera::GetFieldOfView() const
    {
        return m_fieldOfView;
    }

    void Camera::SetFieldOfView(float fieldOfView)
    {
        if (fieldOfView < 1.0f) fieldOfView = 1.0f;
        if (fieldOfView > 180.0f) fieldOfView = 180.0f;
        m_fieldOfView = fieldOfView;
    }

    float Camera::GetNearPlane() const
    {
        return m_nearPlane;
    }

    float Camera::GetFarPlane() const
    {
        return m_farPlane;
    }

    void Camera::SetNearAndFarPlanes(float nearPlane, float farPlane)
    {
        rade::Assert(nearPlane > 0.0f, "Invalid near plane value %f\n", nearPlane);
        rade::Assert(farPlane > nearPlane, "Far plane cannot be <= nearplane\n");
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
    }

    float Camera::GetViewportAspectRatio() const
    {
        return m_viewportAspectRatio;
    }

    void Camera::SetViewportAspectRatio(float viewportAspectRatio)
    {
        rade::Assert(viewportAspectRatio > 0.0, "Invalid aspect ratio\n");
        m_viewportAspectRatio = viewportAspectRatio;
    }

    bool Camera::IsInView(const rade::vector3& point)
    {
        m_frustrum.UpdateFrustrumPlanes(GetMatrix());
        return m_frustrum.IsInsideFrustrum(point);
    }
};
