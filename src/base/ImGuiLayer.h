#pragma once

#include "Base.h"
#include "Layer.h"

namespace Azer
{
    class Renderer;

    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(Renderer* renderer)
            : Layer("ImGuiLayer"), m_Renderer(renderer)
        {
        }
        ~ImGuiLayer() override = default;

        void OnAttach(EngineContext& ctx) override;

        void Begin();
        void End();
    private:
        Renderer* m_Renderer;
    };
}