#include "azpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "SDL3Texture.h"
#include "GPUTexture.h"
#include "VulkanTexture.h"

namespace Azer
{
    Ref<Texture> Texture::Create(const std::string& filePath)
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::SDL_2D:
            return SDL3Texture::Create(filePath);
        case RendererAPI::API::SDL_GPU:
            return GPUTexture::Create(filePath);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanTexture>(filePath);
        default:
            assert(false && "Unsupported RendererAPI");
            return nullptr;
        }
    }

    Ref<Texture> Texture::Create(void* pixels, uint32_t width, uint32_t height)
    {
        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::SDL_2D:
            return SDL3Texture::Create(pixels, width, height);
        case RendererAPI::API::SDL_GPU:
            return GPUTexture::Create(pixels, width, height);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanTexture>(width, height, pixels);
        default:
            assert(false && "Unsupported RendererAPI");
            return nullptr;
        }
    }
}
