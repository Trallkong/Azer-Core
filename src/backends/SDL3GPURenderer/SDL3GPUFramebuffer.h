//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "renderer/Framebuffer.h"
#include "SDL3/SDL.h"

namespace azer
{
    class SDL3GPUFramebuffer : public Framebuffer
    {
    public:
        SDL3GPUFramebuffer(SDL_GPUDevice* device, const FramebufferSpec& spec);
        ~SDL3GPUFramebuffer() override;

        void Resize(uint32_t width, uint32_t height) override;

        Ref<Texture> GetColorTexture() const override;
        void* GetColorTextureHandle() const override;
        void* GetDepthTextureHandle() const override;

        uint32_t GetWidth() const override { return m_Spec.width; }
        uint32_t GetHeight() const override { return m_Spec.height; }

    private:
        void Invalidate();
        void Release();

        SDL_GPUDevice* m_Device = nullptr;
        FramebufferSpec m_Spec;

        SDL_GPUTexture* m_ColorTexture = nullptr;
        SDL_GPUTexture* m_DepthTexture = nullptr;
    };
}
