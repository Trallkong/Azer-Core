//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Base.h"
#include "Components.h"
#include "entt/entt.hpp"
#include <unordered_map>
#include <string>

namespace Azer
{
    class World
    {
    public:
        World() = default;
        ~World() = default;

        // 禁止拷贝
        World(const World&) = delete;
        World& operator=(const World&) = delete;

        // 创建实体
        entt::entity CreateEntity();
        entt::entity CreateEntity(const std::string& name);

        // 销毁实体
        void DestroyEntity(entt::entity entity);

        // 添加组件
        template<typename T, typename... Args>
        T& AddComponent(entt::entity entity, Args&&... args)
        {
            return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        // 移除组件
        template<typename T>
        void RemoveComponent(entt::entity entity)
        {
            m_Registry.remove<T>(entity);
        }

        // 获取组件
        template<typename T>
        T& GetComponent(entt::entity entity)
        {
            return m_Registry.get<T>(entity);
        }

        template<typename T>
        const T& GetComponent(entt::entity entity) const
        {
            return m_Registry.get<T>(entity);
        }

        // 检查组件
        template<typename T>
        bool HasComponent(entt::entity entity) const
        {
            return m_Registry.all_of<T>(entity);
        }

        // 查询实体
        template<typename... Components>
        auto GetAllEntitiesWith()
        {
            return m_Registry.view<Components...>();
        }

        template<typename... Components>
        auto GetAllEntitiesWith() const
        {
            return m_Registry.view<Components...>();
        }

        // 根据名称查找实体
        entt::entity FindEntityByName(const std::string& name);

        // 获取实体数量
        size_t GetEntityCount() const { return m_Registry.view<entt::entity>().size(); }

        // 访问registry（用于高级操作）
        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

        // 清空所有实体
        void Clear();

    private:
        entt::registry m_Registry;
        std::unordered_map<std::string, entt::entity> m_EntityMap;
        uint64_t m_NextID = 1;
    };
}