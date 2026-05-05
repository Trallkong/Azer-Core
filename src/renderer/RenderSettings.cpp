//
// Created by Trallkong on 2026/5/5.
//

#include "RenderSettings.h"

#include "Application.h"

namespace azer
{
    void RenderSettings::SetViewport(const uint32_t width, const uint32_t height, const uint32_t offsetX, const uint32_t offsetY)
    {
        auto* renderer = Application::Get().GetRenderer();
        renderer->SetViewport(width, height, offsetX, offsetY);
    }

    void RenderSettings::SetCamera(const Camera& camera)
    {
        auto* renderer = Application::Get().GetRenderer();
        renderer->SetCamera(camera);
    }
} // azer