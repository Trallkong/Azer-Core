//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "renderer/Framebuffer.h"
#include "SDL3/SDL.h"

namespace Azer
{
    class SDL3Renderer;

    class SDL3Framebuffer : public Framebuffer
    {
    public:
        SDL3Framebuffer(SDL3Renderer* renderer, const FramebufferSpec& spec);
        ~SDL3Framebuffer() override;

        void Resize(uint32_t width, uint32_t height) override;

        Ref<Texture> GetColorTexture() const override;
        void* GetColorTextureHandle() const override;
        void* GetDepthTextureHandle() const override;

        uint32_t GetWidth() const override { return m_Spec.width; }
        uint32_t GetHeight() const override { return m_Spec.height; }

    private:
        void Invalidate();

        SDL3Renderer* m_Renderer = nullptr;
        FramebufferSpec m_Spec;
        SDL_Texture* m_ColorTexture = nullptr;
    };
}
