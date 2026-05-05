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
            return glm::perspective(glm::radians(Fov), viewportW / viewportH, 0.1f, 1000.0f);
        }
        glm::mat4 GetView() const override
        {
            return glm::lookAt(Position, Target, Up);
        }

        float Fov = 45.0f;
        glm::vec3 Position {0.0f, 0.0f, 10};
        glm::vec3 Target { 0.0f, 0.0f, 0.0f };
        glm::vec3 Up { 0.0f, 1.0f, 0.0f };
    };
}

#endif //AZER_DEV_CAMERA3D_H
