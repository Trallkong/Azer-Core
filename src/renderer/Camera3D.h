//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"

#include "Transform3D.h"

namespace azer
{
    class Camera3D : public Camera
    {
    public:
        Camera3D(const Transform3D& transform = {}, float fov = 45.0f)
            : m_Transform(transform), m_Fov(fov) {}

        glm::mat4 GetProjectionMatrix() override
        {
            return glm::perspective(glm::radians(m_Fov), m_AspectRatio, 0.1f, 1000.0f);
        }

        glm::mat4 GetViewMatrix() const override
        {
            return glm::inverse(m_Transform.GetMatrix());
        }

        float GetFov() const { return m_Fov; }
        void SetFov(float fov) { m_Fov = fov; }

        float GetAspectRatio() const { return m_AspectRatio; }
        void SetAspectRatio(float ratio) { m_AspectRatio = ratio; }

        const Transform3D& GetTransform() const { return m_Transform; }
        void SetTransform(const Transform3D& transform) { m_Transform = transform; }

    private:
        glm::mat4 m_Projection;

        Transform3D m_Transform;
        float m_Fov = 45.0f;
        float m_AspectRatio = 16.0f / 9.0f;
    };
}
