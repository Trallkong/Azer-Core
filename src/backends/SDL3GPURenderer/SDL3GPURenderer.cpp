//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3GPURenderer.h"
#include "imgui_impl_sdlgpu3.h"

azer::SDL3GPURenderer::SDL3GPURenderer()
{
}

azer::SDL3GPURenderer::~SDL3GPURenderer()
{
}

bool azer::SDL3GPURenderer::Initialize(SDL_Window* window)
{
    m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    SDL_ClaimWindowForGPUDevice(m_Device, window);
    m_Window = window;
    return m_Device != nullptr;
}

void azer::SDL3GPURenderer::BeginFrame(const glm::vec3& clearColor)
{
    m_ClearColor = clearColor;
}

void azer::SDL3GPURenderer::EndFrame()
{
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_Device);
    if (m_ImGuiDrawData)
        ImGui_ImplSDLGPU3_PrepareDrawData(m_ImGuiDrawData, cmd);

    SDL_GPUTexture* swapchain_texture;
    Uint32 w, h;
    SDL_AcquireGPUSwapchainTexture(cmd, m_Window, &swapchain_texture, &w, &h);
    if (swapchain_texture != nullptr)
    {
        SDL_GPUColorTargetInfo color_target_info = {};
        color_target_info.texture = swapchain_texture;
        SDL_FColor clear_color = {m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f};
        color_target_info.clear_color = clear_color;
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target_info, 1, nullptr);

        if (m_ImGuiDrawData)
            ImGui_ImplSDLGPU3_RenderDrawData(m_ImGuiDrawData, cmd, render_pass, nullptr);

        SDL_EndGPURenderPass(render_pass);
    }
    SDL_SubmitGPUCommandBuffer(cmd);
    m_ImGuiDrawData = nullptr;
}

void azer::SDL3GPURenderer::SetCamera(const CameraConfig& config)
{
}

void azer::SDL3GPURenderer::DrawQuad(float x, float y, float w, float h)
{
}

void azer::SDL3GPURenderer::DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color)
{
}

void azer::SDL3GPURenderer::DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle)
{

}

azer::Scope<azer::Texture> azer::SDL3GPURenderer::CreateTexture(const std::string& filePath)
{
    return nullptr;
}

void azer::SDL3GPURenderer::SetImGuiDrawData(ImDrawData* drawData)
{
    m_ImGuiDrawData = drawData;
}
