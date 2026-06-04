//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "System.h"
#include "Components.h"

namespace azer
{
    class Renderer;

    class RenderSystem : public System
    {
    public:
        RenderSystem() = default;
        ~RenderSystem() override = default;

        void OnInitialize(World& world, EngineContext& context) override;
        void OnRender(World& world) override;
        void OnShutdown() override;

        const char* GetName() const override { return "RenderSystem"; }

    private:
        Renderer* m_Renderer = nullptr;
    };
}