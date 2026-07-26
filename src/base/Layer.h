//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include <string>

#include "EngineContext.h"
#include "event/Event.h"

namespace Azer
{
    /**
     * @brief 层的基类。一个"层"代表应用中的一个独立逻辑/渲染模块。
     *
     * 通过重写虚钩子，层可以接入 Application 的主循环：
     *   OnAttach / OnDetach    — 生命周期
     *   OnPhysicsUpdate        — 固定时间步长（默认 60fps）物理更新
     *   OnUpdate               — 每帧逻辑更新（跟随显示器刷新率）
     *   OnInterpolate          — 物理插值（alpha ∈ [0,1]），发生在 Draw 前
     *   OnEvent                — 事件（逆序分发，覆盖层优先）
     *   OnDraw                 — 绘制
     *   OnImGuiRender          — ImGui UI
     *
     * 在 Application 中，所有层按 Push 顺序依次更新/绘制，
     * 事件则逆序传递（后 Push 的层先处理）。
     *
     * 若层需要自我销毁，调用 RequestRemove() 即可，
     * Application 会在帧末安全地将其 Detach 并回收。
     */
    class Layer {
    public:
        explicit Layer(const std::string& name = "New Layer")
            : m_Name(name)
        {
        }
        virtual ~Layer() = default;

        /// 层被压入 Application 时调用，ctx 持有渲染器与窗口引用
        virtual void OnAttach(EngineContext& ctx) {}
        /// 层被移除前调用，用于释放资源
        virtual void OnDetach() {}
        /// 每帧逻辑更新，delta 为距上一帧的秒数（跟随显示器刷新率）
        virtual void OnUpdate(float delta) {}
        /// 按固定时间步长（默认 1/60s）调用的物理/确定性更新
        virtual void OnPhysicsUpdate(float fixedDelta) {}
        /**
         * @brief 物理插值，在 OnPhysicsUpdate 和 OnDraw 之间调用。
         * @param alpha 当前帧在两次物理 tick 间的进度 [0, 1]
         *
         * 用于平滑渲染：将物体的前一帧状态和当前状态按 alpha 线性插值，
         * 使渲染位置不受固定步长下离散跳变的影响。*/
        virtual void OnInterpolate(float alpha) {}
        /// 每帧绘制，发生在 BeginFrame 与 EndFrame 之间
        virtual void OnDraw() {}
        /// 事件处理，按逆序分发给各层（覆盖层优先）
        virtual void OnEvent(Event& event) {}
        /// ImGui 面板绘制
        virtual void OnImGuiRender() {}

        const std::string& GetName() const { return m_Name; }

        /** @brief 请求在本帧结束时移除当前层。
         *
         *  安全替代方案：不要在 OnUpdate / OnEvent 中直接操作 LayerStack。
         *  调用此方法后，Application 会在帧末调用 OnDetach 并回收内存。*/
        void RequestRemove() { m_PendingRemove = true; }
        bool IsPendingRemove() const { return m_PendingRemove; }
    private:
        std::string m_Name;
        bool m_PendingRemove = false;
    };
}

