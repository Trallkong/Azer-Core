//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include "SDL3/SDL.h"

#include "WindowEvent.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

#include <variant>

namespace Azer
{
    using AnyEvent = std::variant<
        std::monostate,
        WindowCloseEvent, WindowResizeEvent, WindowMinimizedEvent, WindowRestoredEvent,
        KeyPressedEvent, KeyReleasedEvent,
        MouseMovedEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent, MouseScrolledEvent
    >;

    // --- Wrapper Event ---
    class Event
    {
    public:
        AnyEvent data;
        void SetHandled(const bool v) { m_Handled = v; }
        bool IsHandled() const { return m_Handled; }
    private:
        bool m_Handled = false;
    };

    // --- Factory ---
    Event CreateEventFromSDL(const SDL_Event& sdlEvent);
}

