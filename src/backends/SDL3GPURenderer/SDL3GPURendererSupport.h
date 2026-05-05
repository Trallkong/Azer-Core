//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_SDL3GPURENDERERSUPPORT_H
#define AZER_DEV_SDL3GPURENDERERSUPPORT_H

#include "Base.h"
#include "SDL3/SDL.h"

namespace azer
{
    class SDL3GPURenderer;

    class SDL3GPURendererSupport
    {
        friend class SDL3GPURenderer;

        static void CreateGraphicsPipeline(SDL3GPURenderer* renderer);
        static void CreateSampler(SDL3GPURenderer* renderer);
        static void CreateVerticesTransferBuffer(SDL3GPURenderer* renderer);
        static void CreateWhiteTexture(SDL3GPURenderer* renderer);
        static void CreateBuffers(SDL3GPURenderer* renderer);
        static bool PrepareDrawData(SDL3GPURenderer* renderer, SDL_GPUCommandBuffer* cmd);

        static SDL_GPUTransferBuffer* CreateTextureTransferBuffer(SDL_GPUDevice* device, uint32_t width, uint32_t height);
    };
} // azer

#endif //AZER_DEV_SDL3GPURENDERERSUPPORT_H
