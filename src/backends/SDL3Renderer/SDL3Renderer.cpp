//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3Renderer.h"

#include "Camera2D.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "SDL3Texture.h"

azer::SDL3Renderer::SDL3Renderer()
{
}

azer::SDL3Renderer::~SDL3Renderer()
{
    if (m_Renderer)
        SDL_DestroyRenderer(m_Renderer);
}

bool azer::SDL3Renderer::Initialize(Window* window)
{
    m_Renderer = SDL_CreateRenderer(static_cast<SDL_Window*>(window->GetHandle()), nullptr);
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

void azer::SDL3Renderer::SetCamera(const Camera& camera)
{
    // 在 2D 中，相机向右移动，物体就要向左绘图
    const auto& cam = dynamic_cast<const Camera2D&>(camera);
    offsetX = cam.GetX();
    offsetY = cam.GetY();
    zoom = cam.GetZoom();
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

void azer::SDL3Renderer::DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, const float angle)
{
    const SDL_FRect offsetDst = { dst.x - offsetX, dst.y - offsetY, dst.w, dst.h };
    SDL_RenderTextureRotated(m_Renderer, static_cast<SDL_Texture*>(tex->GetHandle()), &src, &offsetDst, angle, nullptr, SDL_FLIP_NONE);
}

azer::Ref<azer::Texture> azer::SDL3Renderer::CreateTexture(const std::string& filePath)
{
    return SDL3Texture::Create(m_Renderer, filePath);
}

azer::Ref<azer::Texture> azer::SDL3Renderer::CreateTexture(void* pixels, uint32_t width, uint32_t height)
{
    return SDL3Texture::Create(m_Renderer, pixels, width, height);
}

azer::Ref<azer::Texture> azer::SDL3Renderer::CreateHDRTexture(const std::string& filePath)
{
    return SDL3Texture::CreateHDR(m_Renderer, filePath);
}

void azer::SDL3Renderer::ImGuiInit(SDL_Window* window)
{
    ImGui_ImplSDL3_InitForSDLRenderer(window, m_Renderer);
    ImGui_ImplSDLRenderer3_Init(m_Renderer);
}

void azer::SDL3Renderer::ImGuiShutdown()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
}

void azer::SDL3Renderer::ImGuiNewFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
}

void azer::SDL3Renderer::SetImGuiDrawData(ImDrawData* drawData)
{
    ImGui_ImplSDLRenderer3_RenderDrawData(drawData, m_Renderer);
}
