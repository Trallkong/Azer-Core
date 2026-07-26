#include "azpch.h"
#include "SDL3Texture.h"

#include "stb_image.h"

namespace Azer
{
    static uint8_t FloatToByte(float value)
    {
        value = glm::clamp(value, 0.0f, 1.0f);
        return static_cast<uint8_t>(value * 255.0f);
    }

    Ref<Texture> SDL3Texture::Create(SDL_Renderer* renderer, const std::string& filePath)
    {
        SDL_Surface* surface = SDL_LoadPNG(filePath.c_str());
        if (!surface)
        {
            AZ_CORE_ERROR("Failed to load texture: {0}", filePath);
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        const uint32_t width = surface->w;
        const uint32_t height = surface->h;
        SDL_DestroySurface(surface);

        if (!texture)
        {
            AZ_CORE_ERROR("Failed to create SDL texture: {0}", SDL_GetError());
            return nullptr;
        }

        return CreateRef<SDL3Texture>(texture, width, height);
    }

    Ref<Texture> SDL3Texture::Create(SDL_Renderer* renderer, void* pixels, uint32_t width, uint32_t height)
    {
        SDL_Surface* surface = SDL_CreateSurfaceFrom(
            static_cast<int>(width),
            static_cast<int>(height),
            SDL_PIXELFORMAT_RGBA8888,
            pixels,
            static_cast<int>(width * 4)
        );
        if (!surface)
        {
            AZ_CORE_ERROR("Failed to create SDL surface from pixels: {0}", SDL_GetError());
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!texture)
        {
            AZ_CORE_ERROR("Failed to create SDL texture from pixels: {0}", SDL_GetError());
            return nullptr;
        }

        return CreateRef<SDL3Texture>(texture, width, height);
    }

    Ref<Texture> SDL3Texture::CreateHDR(SDL_Renderer* renderer, const std::string& filePath)
    {
        int width = 0, height = 0, channels = 0;
        float* hdrPixels = stbi_loadf(filePath.c_str(), &width, &height, &channels, 4);
        if (!hdrPixels)
        {
            AZ_CORE_ERROR("Failed to load HDR texture: {0}", filePath);
            return nullptr;
        }

        std::vector<uint8_t> pixels(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
        for (size_t i = 0; i < pixels.size(); i += 4)
        {
            pixels[i + 0] = FloatToByte(hdrPixels[i + 0]);
            pixels[i + 1] = FloatToByte(hdrPixels[i + 1]);
            pixels[i + 2] = FloatToByte(hdrPixels[i + 2]);
            pixels[i + 3] = FloatToByte(hdrPixels[i + 3]);
        }

        stbi_image_free(hdrPixels);
        return Create(renderer, pixels.data(), static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }
}
