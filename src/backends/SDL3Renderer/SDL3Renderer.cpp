//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3Renderer.h"

#include "imgui_impl_sdlrenderer3.h"
#include "SDL3Texture.h"

azer::SDL3Renderer::SDL3Renderer()
{
}

azer::SDL3Renderer::~SDL3Renderer()
{
}

bool azer::SDL3Renderer::Initialize(SDL_Window* window)
{
    m_Renderer = SDL_CreateRenderer(window, nullptr);
    return m_Renderer != nullptr;
}

void azer::SDL3Renderer::BeginFrame(const glm::vec3& clearColor)
{
    SDL_SetRenderDrawColor(m_Renderer,
        static_cast<Uint8>(clearColor.r * 255),
        static_cast<Uint8>(clearColor.g * 255),
        static_cast<Uint8>(clearColor.b * 255),
        255
    );
    SDL_RenderClear(m_Renderer);
}

void azer::SDL3Renderer::EndFrame()
{
    SDL_RenderPresent(m_Renderer);
}

void azer::SDL3Renderer::SetCamera(const CameraConfig& config)
{
    // 在 2D 中，相机向右移动，物体就要向左绘图
    offsetX = config.x;
    offsetY = config.y;
    zoom = config.zoom;
    SDL_SetRenderScale(m_Renderer, zoom, zoom);
}

void azer::SDL3Renderer::DrawQuad(const float x, const float y, const float w, const float h)
{
    DrawColorQuad(x, y, w, h, {1.0f, 1.0f, 1.0f, 1.0f});
}

void azer::SDL3Renderer::DrawColorQuad(const float x, const float y, const float w, const float h, const glm::vec4& color)
{
    const SDL_FRect rect = { x - offsetX, y - offsetY, w, h };
    SDL_SetRenderDrawColor(m_Renderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    );
    SDL_RenderFillRect(m_Renderer, &rect);
}

void azer::SDL3Renderer::DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle)
{
    const auto* sdlTex = static_cast<SDL3Texture*>(tex);
    SDL_RenderTextureRotated(m_Renderer, static_cast<SDL_Texture*>(sdlTex->GetHandle()), &src, &dst, angle, nullptr, SDL_FLIP_NONE);
}

azer::Scope<azer::Texture> azer::SDL3Renderer::CreateTexture(const std::string& filePath)
{
    SDL_Surface* surf = SDL_LoadPNG(filePath.c_str());
    SDL_Texture* tex = SDL_CreateTextureFromSurface(m_Renderer, surf);
    uint32_t w = surf->w, h = surf->h;
    SDL_DestroySurface(surf);
    return CreateScope<SDL3Texture>(tex, w, h);
}

void azer::SDL3Renderer::SetImGuiDrawData(ImDrawData* drawData)
{
    ImGui_ImplSDLRenderer3_RenderDrawData(drawData, m_Renderer);
}
