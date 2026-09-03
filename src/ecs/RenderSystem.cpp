//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "RenderSystem.h"
#include "World.h"
#include "Renderer2D.h"

namespace Azer
{
    void RenderSystem::OnInitialize(World& world, EngineContext& context)
    {
    }

    void RenderSystem::OnRender(World& world)
    {
        // 查询所有具有TransformComponent和RenderComponent的实体
        auto view = world.GetAllEntitiesWith<TransformComponent, RenderComponent>();
        
        for (auto entity : view)
        {
            auto& transform = view.get<TransformComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);
            
            // 跳过不可见的实体
            if (!render.Visible) continue;
            
            Transform2D t;
            t.Position = glm::vec2(transform.Transform.position.x, transform.Transform.position.y);
            t.Scale = glm::vec2(render.Size.x, render.Size.y);
            t.Rotation = transform.Transform.rotation.value().z;

            if (render.Texture)
            {
                Renderer2D::DrawTexture(render.Texture, t, render.Color.a);
            }
            else
            {
                Renderer2D::DrawColorQuad(t, render.Color);
            }
        }
    }

    void RenderSystem::OnShutdown()
    {
    }
}
