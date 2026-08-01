//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3Renderer.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "SDL3Texture.h"
#include "SDL3Framebuffer.h"

SDL_Renderer* Azer::SDL3Renderer::s_Renderer = nullptr;

bool Azer::SDL3Renderer::Initialize(Window* window)
{
    m_Renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(window->GetHandle()), nullptr);
    s_Renderer = m_Renderer;
    return m_Renderer != nullptr;
}

void Azer::SDL3Renderer::Shutdown()
{
    if (m_Renderer)
        SDL_DestroyRenderer(m_Renderer);
    m_Renderer = nullptr;
    s_Renderer = nullptr;
}

void Azer::SDL3Renderer::BeginFrame(const glm::vec3& clearColor)
{
    SDL_SetRenderDrawColor(m_Renderer,
        static_cast<Uint8>(clearColor.r * 255),
        static_cast<Uint8>(clearColor.g * 255),
        static_cast<Uint8>(clearColor.b * 255),
        255
    );
    SDL_RenderClear(m_Renderer);
}

void Azer::SDL3Renderer::EndFrame()
{
    SDL_RenderPresent(m_Renderer);
}

void Azer::SDL3Renderer::ResetRenderState()
{
    offsetX = 0.0f;
    offsetY = 0.0f;
    zoom = 1.0f;
    SDL_SetRenderScale(m_Renderer, 1.0f, 1.0f);
}

void Azer::SDL3Renderer::SetRenderTarget(Framebuffer* target)
{
    if (target)
        SDL_SetRenderTarget(m_Renderer, static_cast<SDL_Texture*>(target->GetColorTextureHandle()));
    else
        SDL_SetRenderTarget(m_Renderer, nullptr);
}

Azer::Ref<Azer::Framebuffer> Azer::SDL3Renderer::CreateFramebuffer(const FramebufferSpec& spec)
{
    return CreateRef<SDL3Framebuffer>(this, spec);
}

void Azer::SDL3Renderer::ImGuiInit(SDL_Window* window)
{
    ImGui_ImplSDL3_InitForSDLRenderer(window, m_Renderer);
    ImGui_ImplSDLRenderer3_Init(m_Renderer);
}

void Azer::SDL3Renderer::ImGuiShutdown()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
}

void Azer::SDL3Renderer::ImGuiNewFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
}

void Azer::SDL3Renderer::SetImGuiDrawData(ImDrawData* drawData)
{
    ImGui_ImplSDLRenderer3_RenderDrawData(drawData, m_Renderer);
}
