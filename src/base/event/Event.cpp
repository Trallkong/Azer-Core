//
// Created by Trallkong on 2026/5/1.
//

#include "azpch.h"
#include "Event.h"


namespace Azer
{
    Event CreateEventFromSDL(const SDL_Event& sdlEvent)
    {
        AnyEvent data;

        switch (sdlEvent.type)
        {
            case SDL_EVENT_QUIT:
                data = WindowCloseEvent(); break;
            case SDL_EVENT_WINDOW_RESIZED:
                data = WindowResizeEvent(
                    static_cast<int>(sdlEvent.window.data1),
                    static_cast<int>(sdlEvent.window.data2)); break;
            case SDL_EVENT_WINDOW_MINIMIZED :
                data = WindowMinimizedEvent(); break;
            case SDL_EVENT_WINDOW_RESTORED :
                data = WindowRestoredEvent(); break;
            case SDL_EVENT_KEY_DOWN:
                data = KeyPressedEvent(
                    sdlEvent.key.key,
                    sdlEvent.key.repeat); break;
            case SDL_EVENT_KEY_UP:
                data = KeyReleasedEvent(sdlEvent.key.key); break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                data = MouseButtonPressedEvent(sdlEvent.button.button); break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                data = MouseButtonReleasedEvent(sdlEvent.button.button); break;
            case SDL_EVENT_MOUSE_MOTION:
                data = MouseMovedEvent(sdlEvent.motion.x, sdlEvent.motion.y); break;
            case SDL_EVENT_MOUSE_WHEEL:
                data = MouseScrolledEvent(sdlEvent.wheel.x, sdlEvent.wheel.y); break;
            default:
                data = std::monostate{};
        }

        Event event{};
        event.data = data;
        return event;
    }
}
