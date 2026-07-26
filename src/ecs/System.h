//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Base.h"
#include "EngineContext.h"

namespace Azer
{
    class World;

    class System
    {
    public:
        System() = default;
        virtual ~System() = default;

        // 禁止拷贝
        System(const System&) = delete;
        System& operator=(const System&) = delete;

        // 系统生命周期
        virtual void OnInitialize(World& world, EngineContext& context) {}
        virtual void OnUpdate(World& world, float delta) {}
        virtual void OnPhysicsUpdate(World& world, float fixedDelta) {}
        virtual void OnRender(World& world) {}
        virtual void OnShutdown() {}

        // 系统名称（用于调试）
        virtual const char* GetName() const = 0;

        // 系统是否启用
        bool IsEnabled() const { return m_Enabled; }
        void SetEnabled(bool enabled) { m_Enabled = enabled; }

        // 系统优先级（越小越先执行）
        int GetPriority() const { return m_Priority; }
        void SetPriority(int priority) { m_Priority = priority; }

    protected:
        bool m_Enabled = true;
        int m_Priority = 0;
    };
}