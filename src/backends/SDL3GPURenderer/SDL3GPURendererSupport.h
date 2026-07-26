//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include "Base.h"
#include "SDL3/SDL.h"

namespace Azer
{
    class SDL3GPURenderer;

    class SDL3GPURendererSupport
    {
        friend class SDL3GPURenderer;

        static void CreateShaders(SDL3GPURenderer* renderer);
        static void CreateGraphicsPipeline2D(SDL3GPURenderer* renderer);
        static void CreateGraphicsPipeline3D(SDL3GPURenderer* renderer);
        static void CreateGraphicsPipelineSkybox(SDL3GPURenderer* renderer);

        static void CreateSampler(SDL3GPURenderer* renderer);
        static void CreateSkyboxSampler(SDL3GPURenderer* renderer);
        static void CreateVerticesTransferBuffer(SDL3GPURenderer* renderer);
        static void CreateWhiteTexture(SDL3GPURenderer* renderer);
        static void CreateBuffers(SDL3GPURenderer* renderer);
        static bool PrepareDrawData(SDL3GPURenderer* renderer, SDL_GPUCommandBuffer* cmd);
    };
} // azer

