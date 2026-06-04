//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Layer.h"
#include "World.h"
#include "SystemManager.h"

namespace azer
{
    class ECSLayer : public Layer
    {
    public:
        ECSLayer();
        ~ECSLayer() override = default;

        void OnAttach(EngineContext& ctx) override;
        void OnDetach() override;
        void OnUpdate(float delta) override;
        void OnPhysicsUpdate(float fixedDelta) override;
        void OnDraw() override;

        // 访问World和SystemManager
        World& GetWorld() { return m_World; }
        const World& GetWorld() const { return m_World; }
        SystemManager& GetSystemManager() { return m_SystemManager; }
        const SystemManager& GetSystemManager() const { return m_SystemManager; }

    private:
        World m_World;
        SystemManager m_SystemManager;
        EngineContext* m_Context = nullptr;
    };
}