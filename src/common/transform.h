#pragma once

#include <glm/glm.hpp>
#include "point3d.h"

namespace rade
{
    class Transform
    {
    public:

        void SetPosition(const glm::vec3& position)
        {
            m_position = position;
        }

        void OffsetPosition(const glm::vec3& offset)
        {
            m_position += offset;
        }

        void SetHorizontalAngle(const float horizontalAngle)
        {
            m_horizontalAngle = horizontalAngle;
        }

        void SetVerticalAngle(const float verticalAngle)
        {
            m_verticalAngle = verticalAngle;
        }

        void SetRotation(const glm::vec3& rotation)
        {
            m_horizontalAngle = rotation.x;
            m_verticalAngle = rotation.y;
        }

        float GetHorizontalAngle() const
        {
            return m_horizontalAngle;
        }

        float GetVerticalAngle() const
        {
            return m_verticalAngle;
        }

        rade::vector3 GetPosition() const
        {
            return {m_position.x, m_position.y, m_position.z};
        }

        glm::mat4 GetOrientation() const
        {
            glm::mat4 orientation;
            orientation = glm::rotate(orientation, glm::radians(m_verticalAngle), glm::vec3(1, 0, 0));
            orientation = glm::rotate(orientation, glm::radians(m_horizontalAngle), glm::vec3(0, 1, 0));
            return orientation;
        }

//        void OffsetOrientation(float upAngle, float rightAngle)
//        {
//            m_horizontalAngle += rightAngle;
//            m_verticalAngle += upAngle;
//            NormalizeAngles();
//        }
        void OffsetOrientation(float upAngle, float rightAngle)
        {
            m_horizontalAngle += rightAngle;
            m_verticalAngle += upAngle;
            NormalizeAngles();
        }

        void LookAt(glm::vec3 position)
        {
            //rade::Assert(position != m_position,  "Current position and lookat are the same\n");
            glm::vec3 direction = glm::normalize(position - m_position);
            m_verticalAngle = glm::degrees(asinf(-direction.y));
            m_horizontalAngle = -glm::degrees(atan2f(-direction.x, -direction.z));
            NormalizeAngles();
        }

        glm::vec3 ForwardVector() const
        {
            glm::vec4 forward = glm::inverse(GetOrientation()) * glm::vec4(0, 0, -1, 1);
            return glm::vec3(forward);
        }

        glm::vec3 RightVector() const
        {
            glm::vec4 right = glm::inverse(GetOrientation()) * glm::vec4(1, 0, 0, 1);
            return glm::vec3(right);
        }

        glm::vec3 UpVector() const
        {
            glm::vec4 up = glm::inverse(GetOrientation()) * glm::vec4(0, 1, 0, 1);
            return glm::vec3(up);
        }

        glm::mat4 GetMatrix() const
        {
            return GetOrientation() * glm::translate(glm::mat4(), m_position);
        }

        void NormalizeAngles()
        {
            const float MaxVerticalAngle = 88.0f; //must be less than 90 to avoid gimbal lock

            m_horizontalAngle = fmodf(m_horizontalAngle, 360.0f);
            //fmodf can return negative values, but this will make them all positive
            if (m_horizontalAngle < 0.0f)
                m_horizontalAngle += 360.0f;

            if (m_verticalAngle > MaxVerticalAngle)
                m_verticalAngle = MaxVerticalAngle;
            else if (m_verticalAngle < -MaxVerticalAngle)
                m_verticalAngle = -MaxVerticalAngle;
        }

    private:

        glm::vec3 m_position{};
        float m_horizontalAngle = 0.0f;
        float m_verticalAngle = 0.0f;

    };
};
