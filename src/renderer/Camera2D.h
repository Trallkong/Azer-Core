//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"

#include "Transform2D.h"

namespace azer
{
    class Camera2D : public Camera
    {
    public:
        Camera2D() = default;

        Camera2D(const Transform2D& transform, const uint32_t width, const uint32_t height, const float zNear = -1.0, const float zFar = 1.0)
            : m_Transform(transform), m_Width(width), m_Height(height), zNear(zNear), zFar(zFar)
        {
            UpdateProjection();
        }

        glm::mat4 GetProjectionMatrix() override
        {
            UpdateProjection();
            return m_Projection;
        }

        glm::mat4 GetViewMatrix() const override
        {
            return glm::inverse(m_Transform.GetMatrix());
        }

        float GetZoom() const { return m_Zoom; }
        void SetZoom(const float zoom) { m_Zoom = zoom; }

        const Transform2D& GetTransform() const { return m_Transform; }
        void SetTransform(const Transform2D& transform) { m_Transform = transform; }

        static glm::mat4 GetOrthoMatrixFromTransform(
            const Transform2D& transform,
            const float width, const float height, const float zNear, const float zFar)
        {
            const float left = transform.Position.x - width / 2.0f;
            const float right = transform.Position.x + width / 2.0f;
            const float top = transform.Position.y - height / 2.0f;
            const float bottom = transform.Position.y + height / 2.0f;
            return glm::ortho(left, right, bottom, top, zNear, zFar);
        }

    private:
        void UpdateProjection()
        {
            m_Projection = GetOrthoMatrixFromTransform(m_Transform, m_Width / m_Zoom, m_Height / m_Zoom, zNear, zFar);
        }

    private:
        glm::mat4 m_Projection;

        Transform2D m_Transform;
        uint32_t m_Width = 1280;
        uint32_t m_Height = 720;
        float zNear = -1.0f;
        float zFar = 1.0f;

        float m_Zoom = 1.0f;
    };
}