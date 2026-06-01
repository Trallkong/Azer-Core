//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "Base.h"
#include "Transform3D.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include <string>
#include <cstdint>

namespace azer
{
    using GameObjectID = uint64_t;

    class GameObject
    {
    public:
        GameObject();
        explicit GameObject(const std::string& name);
        ~GameObject() = default;

        GameObjectID GetID() const { return m_ID; }

        // Name
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        // Transform (3D — 2D 游戏只用 Position.xy, Rotation.z)
        const Transform3D& GetTransform() const { return m_Transform; }
        Transform3D& GetTransform() { return m_Transform; }
        void SetTransform(const Transform3D& t) { m_Transform = t; }

        // Size (3D bounding: x=width, y=height, z=depth)
        glm::vec3 GetSize() const { return m_Size; }
        void SetSize(const glm::vec3& size) { m_Size = size; }
        void SetSize(float w, float h, float d = 0.0f) { m_Size = {w, h, d}; }

        // Color
        const glm::vec4& GetColor() const { return m_Color; }
        void SetColor(const glm::vec4& color) { m_Color = color; }

        // Texture
        Ref<Texture> GetTexture() const { return m_Texture; }
        void SetTexture(const Ref<Texture>& tex) { m_Texture = tex; }

        // Visibility
        bool IsVisible() const { return m_Visible; }
        void SetVisible(bool visible) { m_Visible = visible; }

    private:
        GameObjectID m_ID;
        std::string m_Name;
        Transform3D m_Transform;
        glm::vec3 m_Size{64.0f, 64.0f, 0.0f};
        glm::vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
        Ref<Texture> m_Texture;
        bool m_Visible = true;

        static GameObjectID s_NextID;
    };
}
