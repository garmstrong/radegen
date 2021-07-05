#pragma once

#include <glm/glm.hpp>

//class CPoint3D;

//class CFrustrum
//{
// public:
//	CFrustrum() = default;
//
//	~CFrustrum() = default;
//
//	void UpdateFrustrumPlanes(const glm::mat4& mat);
//
//	bool IsInside(const CPoint3D& point);
//
// private:
//	CPlane3D m_plane_left;
//	CPlane3D m_plane_right;
//	CPlane3D m_plane_bottom;
//	CPlane3D m_plane_top;
//	CPlane3D m_plane_near;
//	CPlane3D m_plane_far;
//};

/**
 A first-person shooter type of camera.

    Set the properties of the camera, then use the `matrix` method to get the camera matrix for
    use in the vertex shader.

    Includes the perspective projection matrix.
    */
class Camera
{
public:


    Camera();

    /**
     The position of the camera.
        */
    const glm::vec3& position() const;

    void SetPosition(const glm::vec3& position);

    void OffsetPosition(const glm::vec3& offset);

    void SetHorizontalAngle(float horizontalAngle);

    void SetVerticalAngle(float verticalAngle);

    void SetRotation(const glm::vec3& rotation);

    float GetHorizontalAngle() const;

    float GetVerticalAngle() const;

    void SetOrtho(bool ortho)
    {
        m_isOrtho = ortho;
    }

    /**
     The vertical viewing angle of the camera, in degrees.

        Determines how "wide" the view of the camera is. Large angles appear to be zoomed out,
        as the camera has a wide view. Small values appear to be zoomed in, as the camera has a
        very narrow view.

        The value must be between 0 and 180.
        */
    float GetFieldOfView() const;

    void SetFieldOfView(float fieldOfView);

    /**
     The closest visible distance from the camera.

        Objects that are closer to the camera than the near plane distance will not be visible.

        Value must be greater than 0.
        */
    float GetNearPlane() const;

    //glm::vec3 GetPosition() const;

    glm::vec3 GetPosition() const;


    /**
     The farthest visible distance from the camera.

        Objects that are further away from the than the far plane distance will not be visible.

        Value must be greater than the near plane
        */
    float GetFarPlane() const;

    /**
     Sets the near and far plane distances.

        Everything between the near plane and the var plane will be visible. Everything closer
        than the near plane, or farther than the far plane, will not be visible.

        @param nearPlane  Minimum visible distance from camera. Must be > 0
        @param farPlane   Maximum visible distance from camera. Must be > nearPlane
        */
    void SetNearAndFarPlanes(float nearPlane, float farPlane);

    /**
     A rotation matrix that determines the direction the camera is looking.

        Does not include translation (the camera's position).
        */
    glm::mat4 GetOrientation() const;

    /**
     Offsets the cameras orientation.

        The verticle angle is constrained between 85deg and -85deg to avoid gimbal lock.

        @param upAngle     the angle (in degrees) to offset upwards. Negative values are downwards.
        @param rightAngle  the angle (in degrees) to offset rightwards. Negative values are leftwards.
        */
    void OffsetOrientation(float upAngle, float rightAngle);

    /**
     Orients the camera so that is it directly facing `position`

        @param position  the position to look at
        */
    void LookAt(glm::vec3 position);

    /**
     The GetWidth divided by the GetHeight of the screen/window/viewport

        Incorrect values will make the 3D scene look stretched.
        */
    float GetViewportAspectRatio() const;

    void SetViewportAspectRatio(float viewportAspectRatio);

    /** A unit vector representing the direction the camera is facing */
    glm::vec3 ForwardVector() const;

    /** A unit vector representing the direction to the right of the camera*/
    glm::vec3 RightVector() const;

    /** A unit vector representing the direction out of the top of the camera*/
    glm::vec3 UpVector() const;

    /**
     The combined camera transformation matrix, including perspective projection.

        This is the complete matrix to use in the vertex shader.
        */
    glm::mat4 GetMatrix() const;

    /**
     The perspective projection transformation matrix
        */
    glm::mat4 GetProjection() const;

    /**
     The translation and rotation matrix of the camera.

        Same as the `matrix` method, except the return value does not include the projection
        transformation.
        */
    glm::mat4 GetView() const;

    bool m_isOrtho = false;

//	void Update();
//	bool IsInView(const CPoint3D& point);

    void SetViewport(float width, float height)
    {
        m_videoWidth = width;
        m_videoHeight = height;
        m_viewportAspectRatio = m_videoWidth / m_videoHeight;
    }

private:

//	CFrustrum m_frustrum;

    glm::vec3 m_position;
    float m_horizontalAngle;
    float m_verticalAngle;

    float m_fieldOfView;
    float m_nearPlane;
    float m_farPlane;
    float m_viewportAspectRatio;
    float m_videoWidth = 0.0f;
    float m_videoHeight = 0.0f;

    void NormalizeAngles();


};
