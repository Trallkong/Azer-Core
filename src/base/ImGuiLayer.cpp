#include "azpch.h"
#include "ImGuiLayer.h"

#include "Renderer.h"
#include "imgui.h"

void azer::ImGuiLayer::OnAttach(EngineContext& ctx)
{
    Layer::OnAttach(ctx);

    ImGui::CreateContext();

#ifdef AZER_ENABLE_DOCKING
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    m_Renderer->ImGuiInit(static_cast<SDL_Window*>(ctx.window.GetHandle()));
}

void azer::ImGuiLayer::OnDetach()
{
    Layer::OnDetach();
    m_Renderer->ImGuiShutdown();
    ImGui::DestroyContext();
}

void azer::ImGuiLayer::OnUpdate(const float deltaTime)
{
    Layer::OnUpdate(deltaTime);
}

void azer::ImGuiLayer::OnDraw()
{
    Layer::OnDraw();
}

void azer::ImGuiLayer::OnEvent(Event& event)
{
    Layer::OnEvent(event);
}

void azer::ImGuiLayer::OnImGuiRender()
{
    Layer::OnImGuiRender();
}

void azer::ImGuiLayer::Begin()
{
    m_Renderer->ImGuiNewFrame();
    ImGui::NewFrame();
}

void azer::ImGuiLayer::End()
{
    ImGui::Render();
    m_Renderer->SetImGuiDrawData(ImGui::GetDrawData());
}
