//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_CAMERA_H
#define AZER_DEV_CAMERA_H

#include "Base.h"
#include <glm/glm.hpp>

namespace azer
{
    class Camera
    {
    public:
        virtual ~Camera() = default;
        virtual glm::mat4 GetProjection(float viewportW, float viewportH) const = 0;
        virtual glm::mat4 GetView() const = 0;

        glm::mat4 GetViewProjection(const float vpW, const float vpH) const
        {
            return GetProjection(vpW, vpH) * GetView();
        }
    };
}

#endif //AZER_DEV_CAMERA_H
