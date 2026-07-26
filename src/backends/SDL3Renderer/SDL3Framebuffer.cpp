//
// Created by Aier on 2026/5/31.
//

#include "azpch.h"
#include "SDL3Framebuffer.h"
#include "SDL3Renderer.h"

namespace Azer
{
    // 轻量包装：不拥有纹理生命周期，由 Framebuffer 管理
    class SDL3FramebufferTexture : public Texture
    {
    public:
        SDL3FramebufferTexture(SDL_Texture* tex, uint32_t w, uint32_t h)
            : m_Texture(tex), m_Width(w), m_Height(h) {}
        ~SDL3FramebufferTexture() override = default;

        uint32_t GetWidth() const override  { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        void* GetHandle() const override    { return m_Texture; }

    private:
        SDL_Texture* m_Texture;
        uint32_t m_Width, m_Height;
    };

    SDL3Framebuffer::SDL3Framebuffer(SDL3Renderer* renderer, const FramebufferSpec& spec)
        : m_Renderer(renderer), m_Spec(spec)
    {
        Invalidate();
    }

    SDL3Framebuffer::~SDL3Framebuffer()
    {
        if (m_ColorTexture)
            SDL_DestroyTexture(m_ColorTexture);
    }

    void SDL3Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) return;
        if (width == m_Spec.width && height == m_Spec.height) return;

        m_Spec.width = width;
        m_Spec.height = height;
        Invalidate();
    }

    Ref<Texture> SDL3Framebuffer::GetColorTexture() const
    {
        return CreateRef<SDL3FramebufferTexture>(m_ColorTexture, m_Spec.width, m_Spec.height);
    }

    void* SDL3Framebuffer::GetColorTextureHandle() const
    {
        return m_ColorTexture;
    }

    void* SDL3Framebuffer::GetDepthTextureHandle() const
    {
        // SDL3Renderer (2D) 不使用深度
        return nullptr;
    }

    void SDL3Framebuffer::Invalidate()
    {
        if (m_ColorTexture)
            SDL_DestroyTexture(m_ColorTexture);

        m_ColorTexture = SDL_CreateTexture(
            m_Renderer->GetRenderer(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            static_cast<int>(m_Spec.width),
            static_cast<int>(m_Spec.height)
        );

        SDL_SetTextureScaleMode(m_ColorTexture, SDL_SCALEMODE_LINEAR);
    }
}
