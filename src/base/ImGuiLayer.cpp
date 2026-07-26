#include "azpch.h"
#include "ImGuiLayer.h"

#include "Renderer.h"
#include "imgui.h"

void Azer::ImGuiLayer::OnAttach(EngineContext& ctx)
{
    Layer::OnAttach(ctx);

    ImGui::CreateContext();

#ifdef AZER_ENABLE_DOCKING
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    m_Renderer->ImGuiInit(static_cast<SDL_Window*>(ctx.window.GetHandle()));
}

void Azer::ImGuiLayer::OnDetach()
{
    Layer::OnDetach();
    m_Renderer->ImGuiShutdown();
    ImGui::DestroyContext();
}

void Azer::ImGuiLayer::OnUpdate(const float deltaTime)
{
    Layer::OnUpdate(deltaTime);
}

void Azer::ImGuiLayer::OnDraw()
{
    Layer::OnDraw();
}

void Azer::ImGuiLayer::OnEvent(Event& event)
{
    Layer::OnEvent(event);
}

void Azer::ImGuiLayer::OnImGuiRender()
{
    Layer::OnImGuiRender();
}

void Azer::ImGuiLayer::Begin()
{
    m_Renderer->ImGuiNewFrame();
    ImGui::NewFrame();
}

void Azer::ImGuiLayer::End()
{
    ImGui::Render();
    m_Renderer->SetImGuiDrawData(ImGui::GetDrawData());
}
