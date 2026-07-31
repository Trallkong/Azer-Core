#include "azpch.h"
#include "GPUTexture.h"

#include "stb_image.h"

#include "SDL3GPURenderer.h"

namespace Azer
{
    static SDL_GPUTransferBuffer* CreateTextureTransferBuffer(SDL_GPUDevice* device, uint32_t width, uint32_t height, uint32_t bytesPerPixel)
    {
        SDL_GPUTransferBufferCreateInfo info {};
        info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        info.size = bytesPerPixel * width * height;

        SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(device, &info);
        if (!buffer)
            AZ_CORE_ERROR("Failed to create texture transfer buffer: {0}", SDL_GetError());

        return buffer;
    }

    static Ref<Texture> CreateGPUTextureFromPixels(
        SDL_GPUDevice* device,
        void* pixels,
        uint32_t width,
        uint32_t height,
        SDL_GPUTextureFormat format,
        uint32_t bytesPerPixel)
    {
        SDL_GPUTransferBuffer* transferBuffer = CreateTextureTransferBuffer(device, width, height, bytesPerPixel);
        if (!transferBuffer)
            return nullptr;

        SDL_GPUTextureCreateInfo info {};
        info.type        = SDL_GPU_TEXTURETYPE_2D;
        info.format      = format;
        info.width       = width;
        info.height      = height;
        info.num_levels  = 1;
        info.usage       = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        info.layer_count_or_depth = 1;

        SDL_GPUTexture* gpuTexture = SDL_CreateGPUTexture(device, &info);
        if (!gpuTexture)
        {
            AZ_CORE_ERROR("Failed to create GPU texture: {0}", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            return nullptr;
        }

        const Uint32 pixelSize = width * height * bytesPerPixel;
        void* mapped = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
        if (!mapped)
        {
            AZ_CORE_ERROR("Failed to map GPU texture transfer buffer: {0}", SDL_GetError());
            SDL_ReleaseGPUTexture(device, gpuTexture);
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            return nullptr;
        }

        memcpy(mapped, pixels, pixelSize);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
        if (!cmd)
        {
            AZ_CORE_ERROR("Failed to acquire GPU command buffer: {0}", SDL_GetError());
            SDL_ReleaseGPUTexture(device, gpuTexture);
            SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
            return nullptr;
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTextureTransferInfo transferInfo {};
        transferInfo.transfer_buffer = transferBuffer;
        transferInfo.offset = 0;

        SDL_GPUTextureRegion region {};
        region.texture = gpuTexture;
        region.w = width;
        region.h = height;
        region.d = 1;

        SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

        return CreateRef<GPUTexture>(device, gpuTexture, info.format, width, height);
    }

    Ref<Texture> GPUTexture::Create(const std::string& filePath, bool isHDR)
    {
        SDL_GPUDevice* device = SDL3GPURenderer::GetDevice();

        if (isHDR)
        {
            int width = 0, height = 0, channels = 0;
            float* pixels = stbi_loadf(filePath.c_str(), &width, &height, &channels, 4);
            if (!pixels)
            {
                AZ_CORE_ERROR("Failed to load HDR texture: {0}", filePath);
                return nullptr;
            }

            Ref<Texture> texture = CreateGPUTextureFromPixels(
                device,
                pixels,
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height),
                SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT,
                static_cast<uint32_t>(sizeof(float) * 4)
            );

            stbi_image_free(pixels);
            return texture;
        }

        SDL_Surface* loadedSurface = SDL_LoadPNG(filePath.c_str());
        if (!loadedSurface)
        {
            AZ_CORE_ERROR("Failed to load texture: {0}", filePath);
            return nullptr;
        }

        SDL_Surface* surface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(loadedSurface);
        if (!surface)
        {
            AZ_CORE_ERROR("Failed to convert texture surface: {0}", SDL_GetError());
            return nullptr;
        }

        Ref<Texture> texture = CreateGPUTextureFromPixels(
            device,
            surface->pixels,
            static_cast<uint32_t>(surface->w),
            static_cast<uint32_t>(surface->h),
            SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            4
        );

        SDL_DestroySurface(surface);
        return texture;
    }

    Ref<Texture> GPUTexture::Create(void* pixels, uint32_t width, uint32_t height)
    {
        return CreateGPUTextureFromPixels(SDL3GPURenderer::GetDevice(), pixels, width, height, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, 4);
    }
}
