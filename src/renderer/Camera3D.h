//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"

#include "Transform3D.h"

namespace Azer
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
            // 相机朝向 = m_Transform 的 +Z（GetForward）。
            // 标准右手视图（glm::lookAt 同款构造）：view 空间 -Z = 朝向，与 glm::perspective 约定一致。
            // 直接 inverse(变换矩阵) 会让 view 里朝向为 +Z，前方点被透视裁掉 → 不可见。
            glm::vec3 f = glm::normalize(m_Transform.GetForward());
            glm::vec3 s = glm::normalize(glm::cross(f, glm::vec3(0.0f, 1.0f, 0.0f)));  // 右
            glm::vec3 u = glm::cross(s, f);                                           // 上

            glm::mat4 view(1.0f);
            view[0][0] = s.x; view[1][0] = s.y; view[2][0] = s.z;
            view[0][1] = u.x; view[1][1] = u.y; view[2][1] = u.z;
            view[0][2] = -f.x; view[1][2] = -f.y; view[2][2] = -f.z;
            view[3][0] = -glm::dot(s, m_Transform.Position);
            view[3][1] = -glm::dot(u, m_Transform.Position);
            view[3][2] =  glm::dot(f, m_Transform.Position);
            return view;
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
