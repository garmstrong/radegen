#define _USE_MATH_DEFINES

#include <cmath>

#include "camera.h"
#include "osutils.h"

using namespace rade;

static const float MaxVerticalAngle = 88.0f; //must be less than 90 to avoid gimbal lock

//void CFrustrum::UpdateFrustrumPlanes(const glm::mat4& mat)
//{
//	// see Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix
//	// http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
//	// Gil Gribb & Klaus Hartmann
//
//	float left[4];
//	float right[4];
//	float top[4];
//	float bottom[4];
//	float near[4];
//	float far[4];
//
//	for (int i = 4; i--;) left[i] = mat[i][3] + mat[i][0];
//	for (int i = 4; i--;) right[i] = mat[i][3] - mat[i][0];
//	for (int i = 4; i--;) bottom[i] = mat[i][3] + mat[i][1];
//	for (int i = 4; i--;) top[i] = mat[i][3] - mat[i][1];
//	for (int i = 4; i--;) near[i] = mat[i][3] + mat[i][2];
//	for (int i = 4; i--;) far[i] = mat[i][3] - mat[i][2];
//
//	bool normalise = true;
//	m_plane_left.InitFromPlane(left[0], left[1], left[2], left[3], normalise);
//	m_plane_right.InitFromPlane(right[0], right[1], right[2], right[3], normalise);
//	m_plane_bottom.InitFromPlane(bottom[0], bottom[1], bottom[2], bottom[3], normalise);
//	m_plane_top.InitFromPlane(top[0], top[1], top[2], top[3], normalise);
//	m_plane_near.InitFromPlane(near[0], near[1], near[2], near[3], normalise);
//	m_plane_far.InitFromPlane(far[0], far[1], far[2], far[3], normalise);
//}
//
//bool CFrustrum::IsInside(const vector3& point)
//{
//	float radius = 5.0f;
//	if(m_plane_left.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//	if(m_plane_right.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//	if(m_plane_bottom.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//	if(m_plane_top.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//	if(m_plane_near.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//	if(m_plane_far.ClassifyPoint(point, radius) == RMATH::ESide_BACK)
//	{
//		return false;
//	}
//
//
//	return true;
//}

namespace rade
{
    Camera::Camera() :
            m_fieldOfView(50.0f),
            m_nearPlane(0.01f),
            m_farPlane(100.0f),
            m_viewportAspectRatio(4.0f / 3.0f)
    {
    }

//    glm::mat4 Camera::GetOrientation() const
//    {
//        glm::mat4 orientation;
//        orientation = glm::rotate(orientation, glm::radians(m_verticalAngle), glm::vec3(1, 0, 0));
//        orientation = glm::rotate(orientation, glm::radians(m_horizontalAngle), glm::vec3(0, 1, 0));
//        return orientation;
//    }

    glm::mat4 Camera::GetProjection() const
    {
        if (m_isOrtho)
        {
            return glm::ortho(
                    -(m_videoWidth / 2),  // left
                    (m_videoWidth / 2),  // right
                    -(m_videoHeight / 2), // bottom
                    (m_videoHeight / 2), // top
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
        //return GetOrientation() * glm::translate(glm::mat4(), -glm::vec3(m_position.x, m_position.y, m_position.z));
    }

//    void Camera::LookAt(const rade::vector3& position)
//    {
//        glm::vec3 direction = glm::normalize(glm::vec3(position.x, position.y, position.z) - glm::vec3(m_position.x, m_position.y, m_position.z));
//        m_verticalAngle = glm::degrees(asinf(-direction.y));
//        m_horizontalAngle = -glm::degrees(atan2f(-direction.x, -direction.z));
//        NormalizeAngles();
//    }

    glm::mat4 Camera::GetMatrix()
    {
        return GetProjection() * m_transform.GetMatrix();
    }
//
//    void Camera::SetPosition(const rade::vector3& position)
//    {
//        m_transform.SetTranslation(position.GetNegative());
//    }
//
//    void Camera::OffsetPosition(const rade::vector3& offset)
//    {
//        m_transform.OffsetTranslation(offset.GetNegative());
//    }
//
//    void Camera::SetHorizontalAngle(const float horizontalAngle)
//    {
//        //m_horizontalAngle = horizontalAngle;
//        m_transform.SetRotationHorizontal(horizontalAngle);
//    }
//
//    void Camera::SetVerticalAngle(const float verticalAngle)
//    {
//        //m_verticalAngle = verticalAngle;
//        m_transform.SetRotationVertical(verticalAngle);
//    }
//
//    void Camera::SetRotation(const rade::vector3& rotation)
//    {
//        //m_horizontalAngle = rotation.x;
//        //m_verticalAngle = rotation.y;
//        m_transform.SetRotation(rotation);
//    }
//
//    float Camera::GetHorizontalAngle() const
//    {
//        return m_horizontalAngle;
//    }
//
//    float Camera::GetVerticalAngle() const
//    {
//        return m_verticalAngle;
//    }

    float Camera::GetFieldOfView() const
    {
        return m_fieldOfView;
    }

    void Camera::SetFieldOfView(float fieldOfView)
    {
        if (fieldOfView < 0.0f)
            fieldOfView = 0.0f;

        if (fieldOfView > 180.0f)
            fieldOfView = 180.0f;

        m_fieldOfView = fieldOfView;
    }

    float Camera::GetNearPlane() const
    {
        return m_nearPlane;
    }

//    rade::vector3 Camera::GetPosition() const
//    {
//        return m_transform.GetPosition();
//    }

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

//    void Camera::OffsetOrientation(float upAngle, float rightAngle)
//    {
//
//        //m_horizontalAngle += rightAngle;
//        //m_verticalAngle += upAngle;
//        rade::vector3 rot(upAngle, rightAngle, 0.0f);
//        m_transform.OffsetRotation(rot);
//        NormalizeAngles();
//    }

    float Camera::GetViewportAspectRatio() const
    {
        return m_viewportAspectRatio;
    }

    void Camera::SetViewportAspectRatio(float viewportAspectRatio)
    {
        rade::Assert(viewportAspectRatio > 0.0, "Invalid aspect ratio\n");
        m_viewportAspectRatio = viewportAspectRatio;
    }

    void Camera::NormalizeAngles()
    {
//        m_horizontalAngle = fmodf(m_horizontalAngle, 360.0f);
//        //fmodf can return negative values, but this will make them all positive
//        if (m_horizontalAngle < 0.0f)
//            m_horizontalAngle += 360.0f;
//
//        if (m_verticalAngle > MaxVerticalAngle)
//            m_verticalAngle = MaxVerticalAngle;
//        else if (m_verticalAngle < -MaxVerticalAngle)
//            m_verticalAngle = -MaxVerticalAngle;
    }

//void Camera::Update()
//{
//	glm::mat4 mat = GetProjection() * GetView();
//	m_frustrum.UpdateFrustrumPlanes( mat );
//
//}
//
//bool Camera::IsInView(const vector3& point)
//{
//	return m_frustrum.IsInside(point);
//}
};
