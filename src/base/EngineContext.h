#ifndef AZER_ENGINECONTEXT_H
#define AZER_ENGINECONTEXT_H

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

#endif
