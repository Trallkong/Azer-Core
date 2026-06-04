//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "ECSLayer.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"

namespace azer
{
    ECSLayer::ECSLayer()
        : Layer("ECSLayer")
    {
    }

    void ECSLayer::OnAttach(EngineContext& ctx)
    {
        m_Context = &ctx;
        
        // 注册默认系统
        m_SystemManager.RegisterSystem<RenderSystem>();
        m_SystemManager.RegisterSystem<PhysicsSystem>();
        
        // 初始化所有系统
        m_SystemManager.InitializeSystems(m_World, ctx);
    }

    void ECSLayer::OnDetach()
    {
        // 关闭所有系统
        m_SystemManager.ShutdownSystems();
        
        // 清空世界
        m_World.Clear();
        
        m_Context = nullptr;
    }

    void ECSLayer::OnUpdate(float delta)
    {
        // 更新所有系统
        m_SystemManager.UpdateSystems(m_World, delta);
    }

    void ECSLayer::OnPhysicsUpdate(float fixedDelta)
    {
        // 物理更新所有系统
        m_SystemManager.PhysicsUpdateSystems(m_World, fixedDelta);
    }

    void ECSLayer::OnDraw()
    {
        // 渲染所有系统
        m_SystemManager.RenderSystems(m_World);
    }
}