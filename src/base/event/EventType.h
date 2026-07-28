#pragma once

#include "Base.h"

namespace Azer {

    enum class EventType : uint32_t
    {
        None = 0,
        WindowClose, WindowResize, WindowMinimized, WindowRestored,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased,
        MouseMoved, MouseScrolled,
    };
}