//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "GameObjectWrapper.h"

namespace Azer
{
    entt::entity GameObjectWrapper::ConvertToEntity(GameObject& gameObject, World& world)
    {
        // 创建实体
        entt::entity entity = world.CreateEntity(gameObject.GetName());
        
        // 添加ID组件
        auto& idComponent = world.AddComponent<IDComponent>(entity);
        idComponent.ID = gameObject.GetID();
        
        // 添加名称组件
        auto& nameComponent = world.AddComponent<NameComponent>(entity);
        nameComponent.Name = gameObject.GetName();
        
        // 添加变换组件
        auto& transformComponent = world.AddComponent<TransformComponent>(entity);
        transformComponent.Transform = gameObject.GetTransform();
        
        // 添加渲染组件
        auto& renderComponent = world.AddComponent<RenderComponent>(entity);
        renderComponent.Size = gameObject.GetSize();
        renderComponent.Color = gameObject.GetColor();
        renderComponent.Texture = gameObject.GetTexture();
        renderComponent.TexturePath = gameObject.GetTexturePath();
        renderComponent.Visible = gameObject.IsVisible();
        
        return entity;
    }

    Scope<GameObject> GameObjectWrapper::ConvertFromEntity(entt::entity entity, const World& world)
    {
        // 获取组件
        auto& nameComponent = world.GetComponent<NameComponent>(entity);
        auto& transformComponent = world.GetComponent<TransformComponent>(entity);
        auto& renderComponent = world.GetComponent<RenderComponent>(entity);
        
        // 创建GameObject
        auto gameObject = CreateScope<GameObject>(nameComponent.Name);
        
        // 设置属性
        gameObject->SetTransform(transformComponent.Transform);
        gameObject->SetSize(renderComponent.Size);
        gameObject->SetColor(renderComponent.Color);
        gameObject->SetTexture(renderComponent.Texture);
        gameObject->SetTexturePath(renderComponent.TexturePath);
        gameObject->SetVisible(renderComponent.Visible);
        
        return gameObject;
    }

    void GameObjectWrapper::SyncToEntity(GameObject& gameObject, entt::entity entity, World& world)
    {
        // 更新名称组件
        if (world.HasComponent<NameComponent>(entity))
        {
            auto& nameComponent = world.GetComponent<NameComponent>(entity);
            nameComponent.Name = gameObject.GetName();
        }
        
        // 更新变换组件
        if (world.HasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = world.GetComponent<TransformComponent>(entity);
            transformComponent.Transform = gameObject.GetTransform();
        }
        
        // 更新渲染组件
        if (world.HasComponent<RenderComponent>(entity))
        {
            auto& renderComponent = world.GetComponent<RenderComponent>(entity);
            renderComponent.Size = gameObject.GetSize();
            renderComponent.Color = gameObject.GetColor();
            renderComponent.Texture = gameObject.GetTexture();
            renderComponent.TexturePath = gameObject.GetTexturePath();
            renderComponent.Visible = gameObject.IsVisible();
        }
    }

    void GameObjectWrapper::SyncFromEntity(entt::entity entity, GameObject& gameObject, const World& world)
    {
        // 更新名称
        if (world.HasComponent<NameComponent>(entity))
        {
            auto& nameComponent = world.GetComponent<NameComponent>(entity);
            gameObject.SetName(nameComponent.Name);
        }
        
        // 更新变换
        if (world.HasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = world.GetComponent<TransformComponent>(entity);
            gameObject.SetTransform(transformComponent.Transform);
        }
        
        // 更新渲染属性
        if (world.HasComponent<RenderComponent>(entity))
        {
            auto& renderComponent = world.GetComponent<RenderComponent>(entity);
            gameObject.SetSize(renderComponent.Size);
            gameObject.SetColor(renderComponent.Color);
            gameObject.SetTexture(renderComponent.Texture);
            gameObject.SetTexturePath(renderComponent.TexturePath);
            gameObject.SetVisible(renderComponent.Visible);
        }
    }
}