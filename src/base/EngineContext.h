#pragma once

namespace azer
{
    class Renderer;
    class Window;

    struct EngineContext
    {
        Renderer& renderer;
        Window&   window;
    };
}

