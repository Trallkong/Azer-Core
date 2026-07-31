#include "azpch.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "FileSystem.h"

#include "SDL3Texture.h"
#include "GPUTexture.h"
#include "VulkanTexture.h"

namespace Azer
{
    Ref<Texture> Texture::Create(const std::string& filePath)
    {
        // 统一按 FileSystem 的 root path 解析，避免相对路径依赖进程工作目录
        const std::string resolved = FileSystem::ResolvePath(filePath);

        switch (RendererAPI::s_API)
        {
        case RendererAPI::API::SDL_2D:
            return SDL3Texture::Create(resolved);
        case RendererAPI::API::SDL_GPU:
            return GPUTexture::Create(resolved);
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanTexture>(resolved);
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
