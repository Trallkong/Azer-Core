//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include "glm/glm.hpp"

namespace Azer
{
    // 2D 相机 UBO（quad2d shader 的 BufferData）
    struct BufferData
    {
        glm::mat4 viewProjMat = glm::mat4(1.0);
    };

    // 3D 相机 UBO（base3d shader 的 CameraBlock，std140，mat4 需 16 字节对齐）
    struct CameraData
    {
        glm::vec3 position = glm::vec3(0.0f);
        float pad = 0.0f;
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        glm::mat4 projectionMatrix = glm::mat4(1.0f);
    };

    // quad2d 的 push constants
    struct DrawPushConstants
    {
        glm::mat4 modelMat = glm::mat4(1.0);
        glm::vec4 color = glm::vec4(1.0f);
    };
}
