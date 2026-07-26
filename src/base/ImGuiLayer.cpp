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

void Azer::ImGuiLayer::Begin()
{
    m_Renderer->ImGuiNewFrame();
    ImGui::NewFrame();
}

void Azer::ImGuiLayer::End()
{
    ImGui::Render();
}
