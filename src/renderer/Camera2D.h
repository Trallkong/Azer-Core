//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_CAMERA2D_H
#define AZER_DEV_CAMERA2D_H

#include "Base.h"
#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace azer
{
    class Camera2D : public Camera
    {
    public:
        glm::mat4 GetProjection(const float viewportW, const float viewportH) const override
        {
            return glm::ortho(
                0.0f,
                viewportW / m_Zoom,
                viewportH / m_Zoom,
                0.0f, -1.0f, 1.0f);
        }
        glm::mat4 GetView() const override
        {
            return glm::translate(glm::mat4(1.0f), glm::vec3(-m_X, -m_Y, 0.0f));
        }

        float GetX() const { return m_X; }
        void SetX(const float x) { m_X = x; }
        float GetY() const { return m_Y; }
        void SetY(const float y) { m_Y = y; }
        float GetZoom() const { return m_Zoom; }
        void SetZoom(const float zoom) { m_Zoom = zoom; }

    private:
        float m_X = 0.0f;
        float m_Y = 0.0f;
        float m_Zoom = 1.0f;
    };
}

#endif //AZER_DEV_CAMERA2D_H
