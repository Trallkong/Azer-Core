//
// Created by Aier on 2026/5/31.
//

#include "azpch.h"
#include "SDL3GPUFramebuffer.h"
#include "GPUTexture.h"

namespace Azer
{
    // 轻量包装：不拥有纹理生命周期，由 Framebuffer 管理
    class GPUFramebufferTexture : public Texture
    {
    public:
        GPUFramebufferTexture(SDL_GPUTexture* tex, uint32_t w, uint32_t h)
            : m_Texture(tex), m_Width(w), m_Height(h) {}
        ~GPUFramebufferTexture() override = default;

        uint32_t GetWidth() const override  { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        void* GetHandle() const override    { return m_Texture; }

    private:
        SDL_GPUTexture* m_Texture;
        uint32_t m_Width, m_Height;
    };

    SDL3GPUFramebuffer::SDL3GPUFramebuffer(SDL_GPUDevice* device, const FramebufferSpec& spec)
        : m_Device(device), m_Spec(spec)
    {
        Invalidate();
    }

    SDL3GPUFramebuffer::~SDL3GPUFramebuffer()
    {
        Release();
    }

    void SDL3GPUFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) return;
        if (width == m_Spec.width && height == m_Spec.height) return;

        m_Spec.width = width;
        m_Spec.height = height;
        Release();
        Invalidate();
    }

    Ref<Texture> SDL3GPUFramebuffer::GetColorTexture() const
    {
        return CreateRef<GPUFramebufferTexture>(m_ColorTexture, m_Spec.width, m_Spec.height);
    }

    void* SDL3GPUFramebuffer::GetColorTextureHandle() const
    {
        return m_ColorTexture;
    }

    void* SDL3GPUFramebuffer::GetDepthTextureHandle() const
    {
        return m_DepthTexture;
    }

    void SDL3GPUFramebuffer::Invalidate()
    {
        // Color texture: render target + sampleable (for ImGui display)
        SDL_GPUTextureCreateInfo colorInfo{};
        colorInfo.type = SDL_GPU_TEXTURETYPE_2D;
        colorInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        colorInfo.width = m_Spec.width;
        colorInfo.height = m_Spec.height;
        colorInfo.layer_count_or_depth = 1;
        colorInfo.num_levels = 1;
        colorInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;

        m_ColorTexture = SDL_CreateGPUTexture(m_Device, &colorInfo);
        if (!m_ColorTexture)
            AZ_CORE_ERROR("Failed to create framebuffer color texture: {0}", SDL_GetError());

        // Depth texture
        if (m_Spec.hasDepth)
        {
            SDL_GPUTextureCreateInfo depthInfo{};
            depthInfo.type = SDL_GPU_TEXTURETYPE_2D;
            depthInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
            depthInfo.width = m_Spec.width;
            depthInfo.height = m_Spec.height;
            depthInfo.layer_count_or_depth = 1;
            depthInfo.num_levels = 1;
            depthInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;

            m_DepthTexture = SDL_CreateGPUTexture(m_Device, &depthInfo);
            if (!m_DepthTexture)
                AZ_CORE_ERROR("Failed to create framebuffer depth texture: {0}", SDL_GetError());
        }
    }

    void SDL3GPUFramebuffer::Release()
    {
        if (m_ColorTexture)
        {
            SDL_ReleaseGPUTexture(m_Device, m_ColorTexture);
            m_ColorTexture = nullptr;
        }
        if (m_DepthTexture)
        {
            SDL_ReleaseGPUTexture(m_Device, m_DepthTexture);
            m_DepthTexture = nullptr;
        }
    }
}
