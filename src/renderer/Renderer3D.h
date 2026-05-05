//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_RENDERER3D_H
#define AZER_DEV_RENDERER3D_H

#include "Base.h"
#include "glm/vec3.hpp"

namespace azer
{
    class Renderer3D
    {
    public:
        static void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    };
} // azer

#endif //AZER_DEV_RENDERER3D_H
