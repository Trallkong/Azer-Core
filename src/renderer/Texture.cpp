#include "azpch.h"
#include "Texture.h"

#include "Renderer.h"

namespace Azer
{
    Ref<Texture> Texture::Create(Renderer& renderer, const std::string& filePath)
    {
        return renderer.CreateTexture(filePath);
    }

    Ref<Texture> Texture::Create(Renderer& renderer, void* pixels, uint32_t width, uint32_t height)
    {
        return renderer.CreateTexture(pixels, width, height);
    }

    Ref<Texture> Texture::CreateHDR(Renderer& renderer, const std::string& filePath)
    {
        return renderer.CreateHDRTexture(filePath);
    }
}
