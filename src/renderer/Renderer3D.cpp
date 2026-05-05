//
// Created by Trallkong on 2026/5/5.
//

#include "Renderer3D.h"
#include "Application.h"

namespace azer
{
    void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        Application::Get().GetRenderer()->DrawCube(
            position, rotation, scale);
    }
} // azer