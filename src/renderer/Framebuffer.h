//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "Base.h"
#include "Texture.h"

namespace Azer
{
    class Renderer;

    struct FramebufferSpec
    {
        uint32_t width = 1280;
        uint32_t height = 720;
        bool hasDepth = true;
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual Ref<Texture> GetColorTexture() const = 0;
        virtual void* GetColorTextureHandle() const = 0;
        virtual void* GetDepthTextureHandle() const = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        static Ref<Framebuffer> Create(Renderer& renderer, const FramebufferSpec& spec);

    protected:
        Framebuffer() = default;
    };
}
