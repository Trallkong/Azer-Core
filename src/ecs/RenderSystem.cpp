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
            
            // 计算位置和大小
            float x = transform.Transform.Position.x;
            float y = transform.Transform.Position.y;
            float w = render.Size.x;
            float h = render.Size.y;
            
            // 如果有纹理，绘制纹理
            if (render.Texture)
            {
                SDL_FRect src = {0.0f, 0.0f, 
                    static_cast<float>(render.Texture->GetWidth()), 
                    static_cast<float>(render.Texture->GetHeight())};
                SDL_FRect dst = {x, y, w, h};
                
                // 计算旋转角度（绕Z轴）
                float angle = transform.Transform.Rotation.z;
                
                m_Renderer->DrawTexture(render.Texture.get(), src, dst, angle, render.Color.a);
            }
            else
            {
                // 绘制纯色矩形
                m_Renderer->DrawColorQuad(x, y, w, h, render.Color, render.Color.a);
            }
        }
    }

    void RenderSystem::OnShutdown()
    {
        m_Renderer = nullptr;
    }
}