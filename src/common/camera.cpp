#define _USE_MATH_DEFINES

#include <cmath>
#include "glm/gtc/matrix_transform.hpp"
#include "camera.h"


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
//bool CFrustrum::IsInside(const CPoint3D& point)
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

Camera::Camera() :
        m_position(0.0f, 0.0f, 1.0f),
        m_horizontalAngle(0.0f),
        m_verticalAngle(0.0f),
        m_fieldOfView(50.0f),
        m_nearPlane(0.01f),
        m_farPlane(100.0f),
        m_viewportAspectRatio(4.0f / 3.0f)
{
}

const glm::vec3& Camera::position() const
{
    return m_position;
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_position = position;
}

void Camera::OffsetPosition(const glm::vec3& offset)
{
    m_position += offset;
}

void Camera::SetHorizontalAngle(const float horizontalAngle)
{
    m_horizontalAngle = horizontalAngle;
}

void Camera::SetVerticalAngle(const float verticalAngle)
{
    m_verticalAngle = verticalAngle;
}

void Camera::SetRotation(const glm::vec3& rotation)
{
    m_horizontalAngle = rotation.x;
    m_verticalAngle = rotation.y;
}

float Camera::GetHorizontalAngle() const
{
    return m_horizontalAngle;
}

float Camera::GetVerticalAngle() const
{
    return m_verticalAngle;
}

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

glm::vec3 Camera::GetPositionGLM() const
{
    return glm::vec3(m_position.x, m_position.y, m_position.z);
}

CPoint3D Camera::GetPosition() const
{
    return CPoint3D(m_position.x, m_position.y, m_position.z);
}

float Camera::GetFarPlane() const
{
    return m_farPlane;
}

void Camera::SetNearAndFarPlanes(float nearPlane, float farPlane)
{
    assert(nearPlane > 0.0f);
    assert(farPlane > nearPlane);
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

glm::mat4 Camera::GetOrientation() const
{
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(m_verticalAngle), glm::vec3(1, 0, 0));
    orientation = glm::rotate(orientation, glm::radians(m_horizontalAngle), glm::vec3(0, 1, 0));
    return orientation;
}

void Camera::OffsetOrientation(float upAngle, float rightAngle)
{
    m_horizontalAngle += rightAngle;
    m_verticalAngle += upAngle;
    NormalizeAngles();
}

void Camera::LookAt(glm::vec3 position)
{
    assert(position != m_position);
    glm::vec3 direction = glm::normalize(position - m_position);
    m_verticalAngle = glm::degrees(asinf(-direction.y));
    m_horizontalAngle = -glm::degrees(atan2f(-direction.x, -direction.z));
    NormalizeAngles();
}

float Camera::GetViewportAspectRatio() const
{
    return m_viewportAspectRatio;
}

void Camera::SetViewportAspectRatio(float viewportAspectRatio)
{
    assert(viewportAspectRatio > 0.0);
    m_viewportAspectRatio = viewportAspectRatio;
}

glm::vec3 Camera::ForwardVector() const
{
    glm::vec4 forward = glm::inverse(GetOrientation()) * glm::vec4(0, 0, -1, 1);
    return glm::vec3(forward);
}

glm::vec3 Camera::RightVector() const
{
    glm::vec4 right = glm::inverse(GetOrientation()) * glm::vec4(1, 0, 0, 1);
    return glm::vec3(right);
}

glm::vec3 Camera::UpVector() const
{
    glm::vec4 up = glm::inverse(GetOrientation()) * glm::vec4(0, 1, 0, 1);
    return glm::vec3(up);
}

glm::mat4 Camera::GetMatrix() const
{
    return GetProjection() * GetView();
}

glm::mat4 Camera::GetProjection() const
{
    if (m_isOrtho)
    {
        return glm::ortho(-(m_videoWidth / 2), (m_videoWidth / 2), (m_videoHeight / 2), -(m_videoHeight / 2), -1000.0f,
                1000.0f);
    }
    else
    {
        return glm::perspective(glm::radians(m_fieldOfView), m_viewportAspectRatio, m_nearPlane, m_farPlane);
    }
}

glm::mat4 Camera::GetView() const
{
    return GetOrientation() * glm::translate(glm::mat4(), -m_position);
}

void Camera::NormalizeAngles()
{
    m_horizontalAngle = fmodf(m_horizontalAngle, 360.0f);
    //fmodf can return negative values, but this will make them all positive
    if (m_horizontalAngle < 0.0f)
        m_horizontalAngle += 360.0f;

    if (m_verticalAngle > MaxVerticalAngle)
        m_verticalAngle = MaxVerticalAngle;
    else if (m_verticalAngle < -MaxVerticalAngle)
        m_verticalAngle = -MaxVerticalAngle;
}

//void Camera::Update()
//{
//	glm::mat4 mat = GetProjection() * GetView();
//	m_frustrum.UpdateFrustrumPlanes( mat );
//
//}
//
//bool Camera::IsInView(const CPoint3D& point)
//{
//	return m_frustrum.IsInside(point);
//}
