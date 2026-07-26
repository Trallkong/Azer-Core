//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "Base.h"
#include "System.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace Azer
{
    class World;
    struct EngineContext;

    class SystemManager
    {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

        // 禁止拷贝
        SystemManager(const SystemManager&) = delete;
        SystemManager& operator=(const SystemManager&) = delete;

        // 注册系统
        template<typename T, typename... Args>
        T* RegisterSystem(Args&&... args)
        {
            auto system = CreateScope<T>(std::forward<Args>(args)...);
            T* ptr = system.get();
            m_Systems.push_back(std::move(system));
            
            // 按优先级排序
            SortSystems();
            
            return ptr;
        }

        // 初始化所有系统
        void InitializeSystems(World& world, EngineContext& context);

        // 更新所有系统
        void UpdateSystems(World& world, float delta);

        // 物理更新所有系统
        void PhysicsUpdateSystems(World& world, float fixedDelta);

        // 渲染所有系统
        void RenderSystems(World& world);

        // 关闭所有系统
        void ShutdownSystems();

        // 获取系统
        template<typename T>
        T* GetSystem()
        {
            for (auto& system : m_Systems)
            {
                T* ptr = dynamic_cast<T*>(system.get());
                if (ptr)
                {
                    return ptr;
                }
            }
            return nullptr;
        }

        // 移除系统
        template<typename T>
        void RemoveSystem()
        {
            m_Systems.erase(
                std::remove_if(m_Systems.begin(), m_Systems.end(),
                    [](const Scope<System>& system)
                    {
                        return dynamic_cast<T*>(system.get()) != nullptr;
                    }),
                m_Systems.end()
            );
        }

        // 获取系统数量
        size_t GetSystemCount() const { return m_Systems.size(); }

    private:
        void SortSystems()
        {
            std::sort(m_Systems.begin(), m_Systems.end(),
                [](const Scope<System>& a, const Scope<System>& b)
                {
                    return a->GetPriority() < b->GetPriority();
                });
        }

        std::vector<Scope<System>> m_Systems;
    };
}