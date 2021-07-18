#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "point3d.h"
#include "transform.h"

//class vector3;

//class CFrustrum
//{
// public:
//	CFrustrum() = default;
//
//	~CFrustrum() = default;
//
//	void UpdateFrustrumPlanes(const glm::mat4& mat);
//
//	bool IsInside(const vector3& point);
//
// private:
//	plane3d m_plane_left;
//	plane3d m_plane_right;
//	plane3d m_plane_bottom;
//	plane3d m_plane_top;
//	plane3d m_plane_near;
//	plane3d m_plane_far;
//};

namespace rade
{
    class Camera
    {
    public:

        Camera();

        // perspective projection transformation matrix
        glm::mat4 GetProjection() const;

        // translation and rotation matrix of the camera.
        // Same as the `matrix` method, except the return value does not include the projection
        glm::mat4 GetView() const;

        glm::mat4 GetMatrix();

//        void SetPosition(const rade::vector3& position);
//
//        void OffsetPosition(const rade::vector3& offset);
//
//        void SetHorizontalAngle(float horizontalAngle);
//
//        void SetVerticalAngle(float verticalAngle);
//
//        void SetRotation(const rade::vector3& rotation);

//        float GetHorizontalAngle() const;
//
//        float GetVerticalAngle() const;

        void SetOrtho(bool ortho)
        {
            m_isOrtho = ortho;
        }

        // vertical viewing angle of the camera, in degrees.
        float GetFieldOfView() const;

        void SetFieldOfView(float fieldOfView);

        // closest visible distance from the camera
        float GetNearPlane() const;

        //rade::vector3 GetPosition() const;

        // the farthest visible distance from the camera.
        float GetFarPlane() const;

        // near and far plane distances
        void SetNearAndFarPlanes(float nearPlane, float farPlane);

        // Offsets the cameras orientation.
        // vertical angle is constrained between 85deg and -85deg to avoid gimbal lock.
        // upAngle     the angle (in degrees) to offset upwards. Negative values are downwards.
        // rightAngle  the angle (in degrees) to offset rightwards. Negative values are leftwards.
        //void OffsetOrientation(float upAngle, float rightAngle);

        // Orients the camera so that is it directly facing `position`
        //void LookAt(const rade::vector3& position);

        // Width divided by the Height of the screen/window/viewport
        float GetViewportAspectRatio() const;

        void SetViewportAspectRatio(float viewportAspectRatio);

        bool m_isOrtho = false;

//	void Update();
//	bool IsInView(const vector3& point);

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

    private:

//	CFrustrum m_frustrum;

        rade::transform m_transform;
        float m_fieldOfView;
        float m_nearPlane;
        float m_farPlane;
        float m_viewportAspectRatio;
        float m_videoWidth = 0.0f;
        float m_videoHeight = 0.0f;

        void NormalizeAngles();

    };
};
