//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include <string>

#include "EngineContext.h"
#include "event/Event.h"

namespace Azer
{
    class Layer {
    public:
        explicit Layer(const std::string& name = "New Layer")
            : m_Name(name)
        {
        }
        virtual ~Layer() = default;

        virtual void OnAttach(EngineContext& ctx) {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float delta) {}
        virtual void OnPhysicsUpdate(float fixedDelta) {}
        virtual void OnInterpolate(float alpha) {}
        virtual void OnDraw() {}
        virtual void OnEvent(const Event& event) {}
        virtual void OnImGuiRender() {}

        inline const std::string& GetName() const { return m_Name; }

        void RequestRemove() { m_PendingRemove = true; }
        bool IsPendingRemove() const { return m_PendingRemove; }
    private:
        std::string m_Name;
        bool m_PendingRemove = false;
    };
}