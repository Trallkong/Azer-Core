//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "Collision.h"

namespace Azer
{
    void PhysicsSystem::OnInitialize(World& world, EngineContext& context)
    {
        // 初始化物理系统
    }

    void PhysicsSystem::OnPhysicsUpdate(World& world, float fixedDelta)
    {
        // 更新物理
        UpdatePhysics(world, fixedDelta);
        
        // 检查碰撞
        CheckCollisions(world);
    }

    void PhysicsSystem::OnShutdown()
    {
        // 关闭物理系统
    }

    void PhysicsSystem::UpdatePhysics(World& world, float fixedDelta)
    {
        // 查询所有具有TransformComponent和PhysicsComponent的实体
        auto view = world.GetAllEntitiesWith<TransformComponent, PhysicsComponent>();
        
        for (auto entity : view)
        {
            auto& transform = view.get<TransformComponent>(entity);
            auto& physics = view.get<PhysicsComponent>(entity);
            
            // 跳过静态物体
            if (physics.IsStatic) continue;
            
            // 更新速度
            physics.Velocity += physics.Acceleration * fixedDelta;
            
            // 更新位置
            transform.Transform.Position += physics.Velocity * fixedDelta;
            
            // 重置加速度（假设每帧重新施加力）
            physics.Acceleration = glm::vec3(0.0f);
        }
    }

    void PhysicsSystem::CheckCollisions(World& world)
    {
        // 查询所有具有TransformComponent和CollisionComponent的实体
        auto view = world.GetAllEntitiesWith<TransformComponent, CollisionComponent>();
        
        // 简单的碰撞检测（O(n^2)复杂度，实际项目中应使用空间分区）
        std::vector<entt::entity> entities;
        for (auto entity : view)
        {
            entities.push_back(entity);
        }
        
        for (size_t i = 0; i < entities.size(); ++i)
        {
            for (size_t j = i + 1; j < entities.size(); ++j)
            {
                auto& transformA = view.get<TransformComponent>(entities[i]);
                auto& collisionA = view.get<CollisionComponent>(entities[i]);
                auto& transformB = view.get<TransformComponent>(entities[j]);
                auto& collisionB = view.get<CollisionComponent>(entities[j]);
                
                // 检查AABB碰撞
                if (collisionA.ColliderType == CollisionComponent::Type::AABB &&
                    collisionB.ColliderType == CollisionComponent::Type::AABB)
                {
                    glm::vec3 posA = transformA.Transform.Position;
                    glm::vec3 sizeA = collisionA.Size;
                    glm::vec3 posB = transformB.Transform.Position;
                    glm::vec3 sizeB = collisionB.Size;
                    
                    // 检查AABB碰撞
                    bool collision = 
                        posA.x < posB.x + sizeB.x &&
                        posA.x + sizeA.x > posB.x &&
                        posA.y < posB.y + sizeB.y &&
                        posA.y + sizeA.y > posB.y;
                    
                    if (collision)
                    {
                        // 处理碰撞
                        // 这里可以触发碰撞事件或执行碰撞响应
                        AZ_CORE_INFO("Collision detected between entities");
                    }
                }
            }
        }
    }
}