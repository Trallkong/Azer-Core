#pragma once

#include "Base.h"
#include "glm/glm.hpp"
#include <vector>
#include <array>

namespace Azer {

    struct VertexData {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec4 color;
    };

    using Vertices = std::vector<VertexData>;
    using Indices = std::vector<uint32_t>;

    class Mesh2D
    {
    public:
        virtual ~Mesh2D() = default;

        const Vertices& GetVertices() const { return m_Vertices; }
        const Indices& GetIndices() const { return m_Indices; }

    protected:
        virtual void UpdateData() = 0;

        Vertices m_Vertices;
        Indices m_Indices;
    };

    class QuadMesh : public Mesh2D
    {
    public:
        QuadMesh()
            : m_Size{10, 10}, m_Color{0, 0, 0, 0}
        {
            m_Vertices.resize(4);
            m_Indices.resize(6);
            UpdateData();
            m_Indices[0] = 0; m_Indices[1] = 1; m_Indices[2] = 2;
            m_Indices[3] = 2; m_Indices[4] = 3; m_Indices[5] = 0;
        }

        void SetSize(const glm::vec2& size) { m_Size = size; UpdateData(); }
        void SetColor(const glm::vec4& color) { m_Color = color; UpdateData(); }

        const glm::vec2& GetSize() const { return m_Size; }
        const glm::vec4& GetColor() const { return m_Color; }

    private:
        glm::vec2 m_Size;
        glm::vec4 m_Color;

        void UpdateData() override
        {
            const float hx = m_Size.x * 0.5f;
            const float hy = m_Size.y * 0.5f;
            m_Vertices[0] = {{ -hx, -hy, 0 }, { 0, 0 }, m_Color};
            m_Vertices[1] = {{  hx, -hy, 0 }, { 1, 0 }, m_Color};
            m_Vertices[2] = {{  hx,  hy, 0 }, { 1, 1 }, m_Color};
            m_Vertices[3] = {{ -hx,  hy, 0 }, { 0, 1 }, m_Color};
        }
    };
}
