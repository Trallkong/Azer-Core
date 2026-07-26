#pragma once

#include <string>

#include "Base.h"
#include "Layer.h"
#include "Texture.h"

namespace Azer
{
    class Renderer;
    class Window;

    class SplashLayer : public Layer
    {
    public:
        explicit SplashLayer(Renderer& renderer, float duration = 2.0f);
        void OnAttach(EngineContext& ctx) override;
        void OnUpdate(float delta) override;
        void OnDraw() override;
        void OnEvent(Event& event) override;
        void OnImGuiRender() override;

        void SetLogo(const std::string& path);
        void SetEngineName(const std::string& name);
    private:
        float m_duration;
        float m_Elapsed = 0.0f;
        Ref<Texture> m_Logo;
        std::string m_EngineName = "Azer Engine";
        Renderer& m_Renderer;
        Window* m_Window = nullptr;
    };
}
