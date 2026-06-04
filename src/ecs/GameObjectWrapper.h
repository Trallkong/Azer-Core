//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Base.h"
#include "GameObject.h"
#include "World.h"
#include "Components.h"

namespace azer
{
    class GameObjectWrapper
    {
    public:
        // 将GameObject转换为ECS实体
        static entt::entity ConvertToEntity(GameObject& gameObject, World& world);
        
        // 从ECS实体创建GameObject
        static Scope<GameObject> ConvertFromEntity(entt::entity entity, const World& world);
        
        // 同步GameObject属性到ECS实体
        static void SyncToEntity(GameObject& gameObject, entt::entity entity, World& world);
        
        // 同步ECS实体属性到GameObject
        static void SyncFromEntity(entt::entity entity, GameObject& gameObject, const World& world);
    };
}