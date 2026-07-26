//
// Created by Aier on 2026/5/31.
//

#include "azpch.h"
#include "Framebuffer.h"
#include "Renderer.h"

namespace Azer
{
    Ref<Framebuffer> Framebuffer::Create(Renderer& renderer, const FramebufferSpec& spec)
    {
        return renderer.CreateFramebuffer(spec);
    }
}
