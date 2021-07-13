#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace rade
{
    class transform
    {
    public:
        void SetTranslation(const rade::vector3& position)
        {
            m_position = position;
        }

        void SetRotation(const rade::vector3& rotation)
        {
            m_rotation = rotation;
        }

        void SetScale(const rade::vector3& scale)
        {
            m_scale = scale;
        }

        glm::mat4 GetModelMatrix()
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(m_position.x, m_position.y, m_position.z));
            model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
            return model;
        }

    private:
        rade::vector3 m_position;
        rade::vector3 m_rotation;
        rade::vector3 m_scale = {1.0f, 1.0f, 1.0f};
        glm::mat4 m_matrix;
    };
}
