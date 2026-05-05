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
                viewportW / Zoom,
                viewportH / Zoom,
                0.0f, -1.0f, 1.0f);
        }
        glm::mat4 GetView() const override
        {
            return glm::translate(glm::mat4(1.0f), glm::vec3(-X, -Y, 0.0f));
        }

        float X = 0.0f, Y = 0.0f;
        float Zoom = 1.0f;
    };
}

#endif //AZER_DEV_CAMERA2D_H
