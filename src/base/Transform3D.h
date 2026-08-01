//
// Created by Trallkong on 2026/5/31.
//

#pragma once

#include "Base.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Azer
{
    struct Transform3D
    {
        glm::vec3 Position{0.0f, 0.0f, 0.0f};
        glm::vec3 Rotation{0.0f, 0.0f, 0.0f};  // 欧拉角，度为单位 (pitch, yaw, roll)
        glm::vec3 Scale{1.0f, 1.0f, 1.0f};

        glm::mat4 GetMatrix() const
        {
            glm::mat4 mat(1.0f);
            mat = glm::translate(mat, Position);
            mat = glm::rotate(mat, Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));  // yaw
            mat = glm::rotate(mat, Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));  // pitch
            mat = glm::rotate(mat, Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));  // roll
            mat = glm::scale(mat, Scale);
            return mat;
        }

        glm::vec3 GetForward() const
        {
            float pitch = Rotation.x;
            float yaw   = Rotation.y;
            return glm::normalize(glm::vec3(
                cos(pitch) * sin(yaw),
                sin(pitch),
                cos(pitch) * cos(yaw)
            ));
        }

        glm::vec3 GetRight() const
        {
            return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
        }

        glm::vec3 GetUp() const
        {
            return glm::normalize(glm::cross(GetRight(), GetForward()));
        }
    };
}
