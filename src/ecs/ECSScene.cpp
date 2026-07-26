//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "ECSScene.h"
#include "GameObjectWrapper.h"

namespace Azer
{
    entt::entity ECSScene::CreateEntity(const std::string& name)
    {
        entt::entity entity = m_World.CreateEntity(name);
        m_Entities.push_back(entity);
        return entity;
    }

    entt::entity ECSScene::CreateEntityFromGameObject(GameObject& gameObject)
    {
        entt::entity entity = GameObjectWrapper::ConvertToEntity(gameObject, m_World);
        m_Entities.push_back(entity);
        return entity;
    }

    void ECSScene::RemoveEntity(entt::entity entity)
    {
        // 从列表中移除
        auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
        if (it != m_Entities.end())
        {
            m_Entities.erase(it);
        }
        
        // 从World中移除
        m_World.DestroyEntity(entity);
    }

    entt::entity ECSScene::FindEntity(const std::string& name)
    {
        return m_World.FindEntityByName(name);
    }

    entt::entity ECSScene::FindEntityByID(uint64_t id)
    {
        // 查询所有具有IDComponent的实体
        auto view = m_World.GetAllEntitiesWith<IDComponent>();
        
        for (auto entity : view)
        {
            auto& idComponent = view.get<IDComponent>(entity);
            if (idComponent.ID == id)
            {
                return entity;
            }
        }
        
        return entt::null;
    }

    void ECSScene::Clear()
    {
        m_Entities.clear();
        m_World.Clear();
    }
}