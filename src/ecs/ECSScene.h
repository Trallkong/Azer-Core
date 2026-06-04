//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Base.h"
#include "World.h"
#include "Components.h"
#include "GameObject.h"
#include <vector>
#include <string>

namespace azer
{
    class ECSScene
    {
    public:
        ECSScene() = default;
        ~ECSScene() = default;

        // 创建实体
        entt::entity CreateEntity(const std::string& name = "Entity");
        
        // 从GameObject创建实体
        entt::entity CreateEntityFromGameObject(GameObject& gameObject);
        
        // 删除实体
        void RemoveEntity(entt::entity entity);
        
        // 查找实体
        entt::entity FindEntity(const std::string& name);
        entt::entity FindEntityByID(uint64_t id);
        
        // 访问实体
        const std::vector<entt::entity>& GetEntities() const { return m_Entities; }
        size_t GetEntityCount() const { return m_Entities.size(); }
        
        // 访问World
        World& GetWorld() { return m_World; }
        const World& GetWorld() const { return m_World; }
        
        // 清空
        void Clear();

    private:
        World m_World;
        std::vector<entt::entity> m_Entities;
    };
}