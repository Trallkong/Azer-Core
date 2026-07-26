//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "SystemManager.h"

namespace Azer
{
    void SystemManager::InitializeSystems(World& world, EngineContext& context)
    {
        for (auto& system : m_Systems)
        {
            if (system->IsEnabled())
            {
                system->OnInitialize(world, context);
            }
        }
    }

    void SystemManager::UpdateSystems(World& world, float delta)
    {
        for (auto& system : m_Systems)
        {
            if (system->IsEnabled())
            {
                system->OnUpdate(world, delta);
            }
        }
    }

    void SystemManager::PhysicsUpdateSystems(World& world, float fixedDelta)
    {
        for (auto& system : m_Systems)
        {
            if (system->IsEnabled())
            {
                system->OnPhysicsUpdate(world, fixedDelta);
            }
        }
    }

    void SystemManager::RenderSystems(World& world)
    {
        for (auto& system : m_Systems)
        {
            if (system->IsEnabled())
            {
                system->OnRender(world);
            }
        }
    }

    void SystemManager::ShutdownSystems()
    {
        for (auto& system : m_Systems)
        {
            system->OnShutdown();
        }
        m_Systems.clear();
    }
}