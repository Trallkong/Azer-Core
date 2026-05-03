//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "ImGuiLayer.h"

#include "Application.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imgui_impl_sdlgpu3.h"
#include "../backends/SDL3Renderer/SDL3Renderer.h"
#include "../backends/SDL3GPURenderer/SDL3GPURenderer.h"

void azer::ImGuiLayer::OnAttach()
{
    Layer::OnAttach();
    const Application& app = Application::Get();

    // 初始化imgui
    if (app.GetMode() == AppMode::Simple2D)
    {
        const auto renderer = dynamic_cast<SDL3Renderer*>(app.GetRenderer());
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForSDLRenderer(app.GetWindow(), renderer->GetRenderer());
        ImGui_ImplSDLRenderer3_Init(renderer->GetRenderer());
    } else if (app.GetMode() == AppMode::ForwardPlus)
    {
        const auto renderer = dynamic_cast<SDL3GPURenderer*>(app.GetRenderer());
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForSDLGPU(app.GetWindow());
        ImGui_ImplSDLGPU3_InitInfo init_info = {};
        init_info.Device = renderer->GetDevice();
        init_info.ColorTargetFormat = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
        ImGui_ImplSDLGPU3_Init(&init_info);
    } else
    {
        assert(false && "Unsupported AppMode");
    }
}

void azer::ImGuiLayer::OnDetach()
{
    Layer::OnDetach();
    const Application& app = Application::Get();
    if (app.GetMode() == AppMode::Simple2D)
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    } else if (app.GetMode() == AppMode::ForwardPlus)
    {
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }
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

void azer::ImGuiLayer::OnImGuiRender()
{
    Layer::OnImGuiRender();
}

void azer::ImGuiLayer::Begin()
{
    const Application& app = Application::Get();
    if (app.GetMode() == AppMode::Simple2D)
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
    } else if (app.GetMode() == AppMode::ForwardPlus)
    {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
    }
    ImGui::NewFrame();
}

void azer::ImGuiLayer::End()
{
    ImGui::Render();
    const Application& app = Application::Get();
    app.GetRenderer()->SetImGuiDrawData(ImGui::GetDrawData());
}
