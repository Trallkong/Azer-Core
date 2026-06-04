//
// Created by opencode on 2026/6/4.
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
    // 唯一标识符组件
    struct IDComponent
    {
        uint64_t ID;
    };

    // 名称组件
    struct NameComponent
    {
        std::string Name;
    };

    // 变换组件（复用现有的Transform3D）
    struct TransformComponent
    {
        Transform3D Transform;
    };

    // 渲染组件
    struct RenderComponent
    {
        glm::vec3 Size{64.0f, 64.0f, 0.0f};
        glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
        Ref<Texture> Texture;
        std::string TexturePath;  // 用于序列化
        bool Visible = true;
    };

    // 物理组件（用于物理模拟）
    struct PhysicsComponent
    {
        glm::vec3 Velocity{0.0f, 0.0f, 0.0f};
        glm::vec3 Acceleration{0.0f, 0.0f, 0.0f};
        float Mass = 1.0f;
        bool IsStatic = false;
    };

    // 碰撞组件
    struct CollisionComponent
    {
        enum class Type { AABB, Sphere };
        Type ColliderType = Type::AABB;
        glm::vec3 Size{64.0f, 64.0f, 0.0f};  // AABB的尺寸或球体的半径
        bool IsTrigger = false;
    };

    // 动画组件
    struct AnimationComponent
    {
        // 动画状态
        bool IsPlaying = false;
        float CurrentTime = 0.0f;
        float Speed = 1.0f;
        bool Loop = true;
        
        // 动画数据（可以引用动画资源）
        // 这里可以添加动画剪辑引用
    };

    // 标签组件（用于分组和过滤）
    struct TagComponent
    {
        std::string Tag;
    };

    // 层级组件（用于父子关系）
    struct HierarchyComponent
    {
        uint64_t ParentID = 0;  // 0表示无父实体
        std::vector<uint64_t> Children;
    };

    // 相机组件
    struct CameraComponent
    {
        enum class Type { Perspective, Orthographic };
        Type CameraType = Type::Perspective;
        float FOV = 45.0f;
        float NearPlane = 0.1f;
        float FarPlane = 1000.0f;
        float OrthoSize = 10.0f;
        bool IsPrimary = false;
    };

    // 光源组件
    struct LightComponent
    {
        enum class Type { Directional, Point, Spot };
        Type LightType = Type::Directional;
        glm::vec3 Color{1.0f, 1.0f, 1.0f};
        float Intensity = 1.0f;
        float Range = 10.0f;
        float SpotAngle = 45.0f;
    };
}