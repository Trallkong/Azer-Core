#pragma once

#include "Base.h"
#include "glm/glm.hpp"
#include <vector>

namespace Azer {

    struct VertexData {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec4 color;
    };

    using Vertices = std::vector<VertexData>;
    using Indices = std::vector<uint32_t>;

    class VulkanMesh 
    {
    public:
        virtual ~VulkanMesh() = default;

        inline Vertices GetVertices() { return m_Vertices; };
        inline const Vertices& GetVertices() const { return m_Vertices; };
        inline Indices GetIndices() { m_Indices; };
        inline const Indices& GetIndices() const { return m_Indices; };

    private:
        virtual void UpdateData() = 0;

    protected:
        Vertices m_Vertices;
        Indices m_Indices;
    };

    class QuadMesh : public VulkanMesh
    {
    public:
        QuadMesh() 
        {
            UpdateData();
            m_Indices = { 0, 1, 2, 2, 3, 0 };
        }
        ~QuadMesh() override = default;

        inline void SetSize(const glm::vec2& size) { m_Size = size; UpdateData(); }
        inline void SetColor(const glm::vec4& color) { m_Color = color; UpdateData(); }

        inline const glm::vec2& GetSize() const { return m_Size; }
        inline const glm::vec4& GetColor() const { return m_Color; }

    private:
        glm::vec2 m_Size = { 10, 10 };
        glm::vec4 m_Color = { 0, 0, 0, 0 };

    private:
        void UpdateData() override
        {
            m_Vertices.clear();
            m_Vertices.push_back({{ 0, 0, 0 }, { 0, 0 }, m_Color});
            m_Vertices.push_back({{ m_Size.x, 0, 0}, { 1, 0 }, m_Color});
            m_Vertices.push_back({{ m_Size.x, m_Size.y, 0}, { 1, 1 }, m_Color});
            m_Vertices.push_back({{ 0, m_Size.y, 0}, { 0, 1 }, m_Color});
        }
    };
}