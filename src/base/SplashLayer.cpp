//
// Created by Trallkong on 2026/5/2.
//

#include "azpch.h"
#include "SplashLayer.h"

#include "Application.h"
#include "Renderer2D.h"

namespace azer
{
    SplashLayer::SplashLayer(const float duration)
        : Layer("SplashLayer") ,m_duration(duration)
    {
    }

    void SplashLayer::OnAttach()
    {
        Layer::OnAttach();
        if (m_Logo == nullptr)
            m_Logo = Renderer2D::CreateTexture("./assets/azer_logo.png");
    }

    void SplashLayer::OnUpdate(const float delta)
    {
        Layer::OnUpdate(delta);
        m_Elapsed += delta;
        if (m_Elapsed >= m_duration)
            Application::Get().PopOverlay();
    }

    void SplashLayer::OnDraw()
    {
        Layer::OnDraw();
    }

    void SplashLayer::OnEvent(Event& event)
    {
        Layer::OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>([](const KeyPressedEvent& e)
        {
            // 按任意键跳过
            Application::Get().PopOverlay();
            return true;
        });
    }

    void SplashLayer::OnImGuiRender()
    {
        Layer::OnImGuiRender();
        auto* dl = ImGui::GetBackgroundDrawList();
        dl->AddRectFilled(ImVec2(0 , 0), ImVec2(1280, 720), IM_COL32(0, 0, 0, 255));
        if (m_Logo)
            dl->AddImage(m_Logo->GetHandle(), ImVec2(0, 0), ImVec2(1280, 720));
    }

    void SplashLayer::SetLogo(const std::string& path)
    {
        m_Logo = Renderer2D::CreateTexture(path);
    }

    void SplashLayer::SetEngineName(const std::string& name)
    {
        m_EngineName = name;
    }
}
