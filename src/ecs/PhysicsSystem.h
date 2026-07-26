//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "System.h"
#include "Components.h"

namespace Azer
{
    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem() = default;
        ~PhysicsSystem() override = default;

        void OnInitialize(World& world, EngineContext& context) override;
        void OnPhysicsUpdate(World& world, float fixedDelta) override;
        void OnShutdown() override;

        const char* GetName() const override { return "PhysicsSystem"; }

    private:
        void UpdatePhysics(World& world, float fixedDelta);
        void CheckCollisions(World& world);
    };
}