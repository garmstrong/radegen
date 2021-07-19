#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace rade
{
    class transform
    {
    public:
        transform()
        {
            m_translationMatrix = glm::mat4(1.0f);
            m_orientationMatrix = glm::mat4(1.0f);
            m_scaleMatrix = glm::mat4(1.0f);
        }

        void OffsetTranslation(const rade::vector3& position)
        {
            m_position = m_position + position;
            CalcTranslation();
        }

        void SetTranslation(const rade::vector3& position)
        {
            m_position = position;
            CalcTranslation();
        }

        void OffsetRotation(const rade::vector3& rotation)
        {
            m_rotation = m_rotation + rotation;
            CalcOrientation();
        }

        void SetRotation(const rade::vector3& rotation)
        {
            m_rotation = rotation;
            CalcOrientation();
        }

        void SetRotationHorizontal(float hAngleDeg)
        {
            m_rotation.x = glm::radians(hAngleDeg);
            CalcOrientation();
        }

        void SetRotationVertical(float vAngleDeg)
        {
            if(vAngleDeg > MaxVerticalAngle) vAngleDeg = MaxVerticalAngle;
            if(vAngleDeg < -MaxVerticalAngle) vAngleDeg = -MaxVerticalAngle;

            m_rotation.y = glm::radians(vAngleDeg);
            CalcOrientation();
        }

        void SetRotationRoll(float rAngleDeg)
        {
            m_rotation.z = glm::radians(rAngleDeg);
            CalcOrientation();
        }

        void SetScale(const rade::vector3& scale)
        {
            m_scale = scale;
            CalcScale();
        }

        glm::mat4 GetTranslationMatrix() const { return m_translationMatrix; }
        glm::mat4 GetOrientationMatrix() const { return m_orientationMatrix; }
        glm::mat4 GetScaleMatrix() const { return m_scaleMatrix; }

        rade::vector3 GetPosition() const { return m_position; }
        rade::vector3 GetRotation() const { return m_rotation; }

        glm::mat4 GetMatrix() const
        {
            return GetScaleMatrix() * GetOrientationMatrix() * GetTranslationMatrix();
        }

        rade::vector3 ForwardVector() const
        {
            glm::vec4 fwd = glm::inverse(GetOrientationMatrix()) * glm::vec4(0, 0, 1, 1);
            return {fwd.x, fwd.y, fwd.z};
        }

        rade::vector3 RightVector() const
        {
            glm::vec4 right = glm::inverse(GetOrientationMatrix()) * glm::vec4(-1, 0, 0, 1);
            return {right.x, right.y, right.z};
        }

        rade::vector3 UpVector() const
        {
            glm::vec4 up = glm::inverse(GetOrientationMatrix()) * glm::vec4(0, -1, 0, 1);
            return {up.x, up.y, up.z};
        }

        void LookAt(const rade::vector3& position)
        {
            glm::vec3 direction = glm::normalize(glm::vec3(position.x, position.y, position.z) - glm::vec3(m_position.x, m_position.y, m_position.z));
            float vAngle = glm::degrees(asinf(-direction.y));
            float hAngle = -glm::degrees(atan2f(-direction.x, -direction.z));
            vAngle = std::min(vAngle, MaxVerticalAngle);
            SetRotationHorizontal(hAngle);
            SetRotationVertical(vAngle);
        }

    private:

        static constexpr float MaxVerticalAngle = 88.0f; //must be less than 90 to avoid gimbal lock

        void CalcScale()
        {
            m_scaleMatrix = glm::mat4(1.0f);
            m_scaleMatrix = glm::scale(m_scaleMatrix, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
        }

        void CalcOrientation()
        {
            m_orientationMatrix = glm::mat4(1.0f);
            m_orientationMatrix = glm::rotate(m_orientationMatrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
            m_orientationMatrix = glm::rotate(m_orientationMatrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
            m_orientationMatrix = glm::rotate(m_orientationMatrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
        }

        void CalcTranslation()
        {
            m_translationMatrix = glm::mat4(1.0f);
            m_translationMatrix = glm::translate(m_translationMatrix, glm::vec3(m_position.x, m_position.y, m_position.z));
        }

        rade::vector3 m_position;
        rade::vector3 m_rotation;
        rade::vector3 m_scale = {1.0f, 1.0f, 1.0f};

        glm::mat4 m_translationMatrix;
        glm::mat4 m_orientationMatrix;
        glm::mat4 m_scaleMatrix;
    };
}
