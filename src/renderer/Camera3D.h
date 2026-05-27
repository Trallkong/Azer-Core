//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_CAMERA3D_H
#define AZER_DEV_CAMERA3D_H
#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace azer
{
    class Camera3D : public Camera
    {
    public:
        glm::mat4 GetProjection(const float viewportW, const float viewportH) const override
        {
            return glm::perspective(glm::radians(m_Fov), viewportW / viewportH, 0.1f, 1000.0f);
        }
        glm::mat4 GetView() const override
        {
            return glm::lookAt(m_Position, m_Target, m_Up);
        }

        float GetFov() const { return m_Fov; }
        void SetFov(const float fov) { m_Fov = fov; }
        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& pos) { m_Position = pos; }
        const glm::vec3& GetTarget() const { return m_Target; }
        void SetTarget(const glm::vec3& target) { m_Target = target; }
        const glm::vec3& GetUp() const { return m_Up; }
        void SetUp(const glm::vec3& up) { m_Up = up; }

    private:
        float m_Fov = 45.0f;
        glm::vec3 m_Position{0.0f, 0.0f, 10.0f};
        glm::vec3 m_Target{0.0f, 0.0f, 0.0f};
        glm::vec3 m_Up{0.0f, 1.0f, 0.0f};
    };
}

#endif //AZER_DEV_CAMERA3D_H
