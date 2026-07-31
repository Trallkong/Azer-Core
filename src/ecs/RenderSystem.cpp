//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "RenderSystem.h"
#include "World.h"
#include "Renderer.h"

namespace Azer
{
    void RenderSystem::OnInitialize(World& world, EngineContext& context)
    {
        m_Renderer = &context.renderer;
    }

    void RenderSystem::OnRender(World& world)
    {
        if (!m_Renderer) return;

        // 查询所有具有TransformComponent和RenderComponent的实体
        auto view = world.GetAllEntitiesWith<TransformComponent, RenderComponent>();
        
        for (auto entity : view)
        {
            auto& transform = view.get<TransformComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);
            
            // 跳过不可见的实体
            if (!render.Visible) continue;
            
            Transform2D t;
            t.Position = glm::vec2(transform.Transform.Position.x, transform.Transform.Position.y);
            t.Scale = glm::vec2(render.Size.x, render.Size.y);
            t.Rotation = transform.Transform.Rotation.z;

            if (render.Texture)
            {
                m_Renderer->DrawTexture(render.Texture, t, render.Color.a);
            }
            else
            {
                m_Renderer->DrawColorQuad(t, render.Color);
            }
        }
    }

    void RenderSystem::OnShutdown()
    {
        m_Renderer = nullptr;
    }
}