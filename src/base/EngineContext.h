#pragma once

namespace Azer
{
    class Renderer;
    class Window;

    struct EngineContext
    {
        Renderer& renderer;
        Window&   window;
    };
}

