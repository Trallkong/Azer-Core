#pragma once
#include "Base.h"
#include "glm/glm.hpp"

namespace azer
{
    struct Material
    {
        glm::vec4 BaseColorFactor = glm::vec4(1.0f);
        float MetallicFactor = 0.0f;
        float RoughnessFactor = 1.0f;
        int32_t BaseColorTexIndex = -1;
        int32_t MetallicRoughnessTexIndex = -1;
    };
}

