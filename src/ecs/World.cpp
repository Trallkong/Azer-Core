//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "World.h"

namespace azer
{
    entt::entity World::CreateEntity()
    {
        entt::entity entity = m_Registry.create();
        
        // 添加ID组件
        auto& idComponent = m_Registry.emplace<IDComponent>(entity);
        idComponent.ID = m_NextID++;
        
        return entity;
    }

    entt::entity World::CreateEntity(const std::string& name)
    {
        entt::entity entity = CreateEntity();
        
        // 添加名称组件
        auto& nameComponent = m_Registry.emplace<NameComponent>(entity);
        nameComponent.Name = name;
        
        // 添加到名称映射
        m_EntityMap[name] = entity;
        
        return entity;
    }

    void World::DestroyEntity(entt::entity entity)
    {
        // 如果有名称组件，从映射中移除
        if (m_Registry.all_of<NameComponent>(entity))
        {
            auto& nameComponent = m_Registry.get<NameComponent>(entity);
            m_EntityMap.erase(nameComponent.Name);
        }
        
        m_Registry.destroy(entity);
    }

    entt::entity World::FindEntityByName(const std::string& name)
    {
        auto it = m_EntityMap.find(name);
        if (it != m_EntityMap.end())
        {
            // 验证实体是否仍然有效
            if (m_Registry.valid(it->second))
            {
                return it->second;
            }
            else
            {
                // 实体已无效，从映射中移除
                m_EntityMap.erase(it);
            }
        }
        return entt::null;
    }

    void World::Clear()
    {
        m_Registry.clear();
        m_EntityMap.clear();
        m_NextID = 1;
    }
}