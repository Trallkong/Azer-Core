//
// Created by Trallkong on 2026/5/31.
//

#include "azpch.h"
#include "Renderer.h"

#include "SDL3GPURenderer.h"
#include "SDL3Renderer.h"

namespace azer
{
    Scope<Renderer> Renderer::CreateRendererFromAppMode(const AppMode& mode)
    {
        switch (mode)
        {
        case AppMode::Simple2D:
            return CreateScope<SDL3Renderer>();
        case AppMode::ForwardPlus:{
            return CreateScope<SDL3GPURenderer>();
        }
        default:
            assert(false && "Unsupported AppMode");
            return nullptr;
        }
    }
}
