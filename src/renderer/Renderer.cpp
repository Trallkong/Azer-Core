//
// Created by Trallkong on 2026/5/31.
//

#include "azpch.h"
#include "Renderer.h"
#include "RendererAPI.h"

#include "SDL3Renderer.h"
#include "VulkanRenderer.h"


namespace Azer
{
    Scope<Renderer> Renderer::Create()
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::SDL_2D:
            return CreateScope<SDL3Renderer>();
        case RendererAPI::API::Vulkan:
            return CreateScope<VulkanRenderer>();
        default:
            assert(false && "Unsupported RendererAPI");
            return nullptr;
        }
    }
}
