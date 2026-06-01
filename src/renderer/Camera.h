//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include "Base.h"
#include <glm/glm.hpp>

namespace azer
{
    class Camera
    {
    public:
        virtual ~Camera() = default;
        virtual glm::mat4 GetProjectionMatrix() = 0;
        virtual glm::mat4 GetViewMatrix() const = 0;

        glm::mat4 GetViewProjectionMatrix()
        {
            return GetProjectionMatrix() * GetViewMatrix();
        }
    };
}

