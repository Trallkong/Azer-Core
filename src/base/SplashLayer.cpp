#include "azpch.h"
#include "SplashLayer.h"

#include "FileSystem.h"
#include "Renderer.h"

namespace azer
{
    SplashLayer::SplashLayer(Renderer& renderer, const float duration)
        : Layer("SplashLayer"), m_duration(duration), m_Renderer(renderer)
    {
    }

    void SplashLayer::OnAttach(EngineContext& ctx)
    {
        Layer::OnAttach(ctx);
        m_Window = &ctx.window;
        if (m_Logo == nullptr)
            m_Logo = Texture::Create(m_Renderer, FileSystem::ResolvePath("./assets/azer_logo.png"));
    }

    void SplashLayer::OnUpdate(const float delta)
    {
        Layer::OnUpdate(delta);
        m_Elapsed += delta;
        if (m_Elapsed >= m_duration)
            RequestRemove();
    }

    void SplashLayer::OnDraw()
    {
        Layer::OnDraw();
    }

    void SplashLayer::OnEvent(Event& event)
    {
        Layer::OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& e)
        {
            RequestRemove();
            return true;
        });
    }

    void SplashLayer::OnImGuiRender()
    {
        Layer::OnImGuiRender();
        auto* dl = ImGui::GetForegroundDrawList(ImGui::GetMainViewport());
        const auto [ww, wh] = m_Window->GetWindowSize();
        const ImVec2 winSize = ImVec2(static_cast<float>(ww), static_cast<float>(wh));
        dl->AddRectFilled(ImVec2(0, 0), winSize, IM_COL32(0, 0, 0, 255));
        if (m_Logo)
            dl->AddImage(m_Logo->GetHandle(), ImVec2(0, 0), winSize);
    }

    void SplashLayer::SetLogo(const std::string& path)
    {
        m_Logo = Texture::Create(m_Renderer, path);
    }

    void SplashLayer::SetEngineName(const std::string& name)
    {
        m_EngineName = name;
    }
}
