//
// Created by Trallkong on 2026/5/1.
//

#include "azpch.h"
#include "Event.h"


namespace azer
{
    Scope<Event> CreateEventFromSDL(const SDL_Event& sdlEvent)
    {
        switch (sdlEvent.type)
        {
            case SDL_EVENT_QUIT:
                return CreateScope<WindowCloseEvent>();
            case SDL_EVENT_WINDOW_RESIZED:
                return CreateScope<WindowResizeEvent>(
                    static_cast<int>(sdlEvent.window.data1),
                    static_cast<int>(sdlEvent.window.data2));
            case SDL_EVENT_KEY_DOWN:
                return CreateScope<KeyPressedEvent>(
                    sdlEvent.key.key,
                    sdlEvent.key.repeat);
            case SDL_EVENT_KEY_UP:
                return CreateScope<KeyReleasedEvent>(sdlEvent.key.key);
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                return CreateScope<MouseButtonPressedEvent>(sdlEvent.button.button);
            case SDL_EVENT_MOUSE_BUTTON_UP:
                return CreateScope<MouseButtonReleasedEvent>(sdlEvent.button.button);
            case SDL_EVENT_MOUSE_MOTION:
                return CreateScope<MouseMovedEvent>(sdlEvent.motion.x, sdlEvent.motion.y);
            case SDL_EVENT_MOUSE_WHEEL:
                return CreateScope<MouseScrolledEvent>(sdlEvent.wheel.x, sdlEvent.wheel.y);
            default:
                return nullptr;
        }
    }
}
