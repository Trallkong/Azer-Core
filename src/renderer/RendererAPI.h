#pragma once

namespace Azer {

    class RendererAPI {
    public:
        enum class API {
            None = 0,
            SDL_2D,
            SDL_GPU,
            Vulkan
        };

        static API s_API;
    };
}