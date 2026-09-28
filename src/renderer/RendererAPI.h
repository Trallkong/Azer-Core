#pragma once

namespace Azer {

    class RendererAPI {
    public:
        enum class API {
            None = 0,
            Vulkan
        };

        static API s_API;
    };
}