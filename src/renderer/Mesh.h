#pragma once
#include "Base.h"
#include "glm/glm.hpp"

namespace azer
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

    struct Mesh
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
        uint32_t MaterialIndex = 0;
    };
}

