//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_RENDERSETTINGS_H
#define AZER_DEV_RENDERSETTINGS_H

#include "Base.h"
#include "Camera.h"

namespace azer
{
    class RenderSettings
    {
    public:
        static void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY);
        static void SetCamera(const Camera& camera);
    };
} // azer

#endif //AZER_DEV_RENDERSETTINGS_H
