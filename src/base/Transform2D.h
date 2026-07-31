//
// Created by Trallkong on 2026/5/31.
//

#pragma once

#include "Base.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Azer
{
    struct Transform2D
    {
        glm::vec2 Position{0.0f, 0.0f};
        float Rotation = 0.0f;
        glm::vec2 Scale{1.0f, 1.0f};

        glm::mat4 GetMatrix() const
        {
            glm::mat4 mat(1.0f);
            mat = glm::translate(mat, glm::vec3(Position, 0.0f));
            mat = glm::rotate(mat, Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
            mat = glm::scale(mat, glm::vec3(Scale, 1.0f));
            return mat;
        }
    };
}
