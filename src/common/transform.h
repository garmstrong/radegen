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
            m_matrix = glm::mat4(1.0f);
        }

        void SetTranslation(const rade::vector3& position)
        {
            m_position = position;
            m_dirty = true;
        }

        void SetRotation(const rade::vector3& rotation)
        {
            m_rotation = rotation;
            m_dirty = true;
        }

        void SetScale(const rade::vector3& scale)
        {
            m_scale = scale;
        }

        glm::mat4 GetModelMatrix()
        {
            if(m_dirty)
            {
                m_matrix = glm::mat4(1.0f);
                m_matrix = glm::translate(m_matrix, glm::vec3(m_position.x, m_position.y, m_position.z));
                m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
                m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
                m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
                m_matrix = glm::scale(m_matrix, glm::vec3(m_scale.x, m_scale.y, m_scale.z));
            }
            return m_matrix;
        }

    private:
        rade::vector3 m_position;
        rade::vector3 m_rotation;
        rade::vector3 m_scale = {1.0f, 1.0f, 1.0f};
        bool m_dirty = false;
        glm::mat4 m_matrix;
    };
}
