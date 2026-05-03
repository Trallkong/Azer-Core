//
// Created by Trallkong on 2026/5/1.
//

#ifndef AZER_EVENT_H
#define AZER_EVENT_H

#include "Base.h"
#include "SDL3/SDL.h"

namespace azer
{
    enum class EventType : uint32_t
    {
        None = 0,
        WindowClose, WindowResize,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased,
        MouseMoved, MouseScrolled,
    };

    // --- Base Event ---
    class Event
    {
    public:
        virtual ~Event() = default;
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        bool Handled = false;
    };

    // --- Window Events ---
    class WindowCloseEvent : public Event
    {
    public:
        static EventType GetStaticType() { return EventType::WindowClose; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "WindowClose"; }
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(int width, int height)
            : m_Width(width), m_Height(height) {}
        static EventType GetStaticType() { return EventType::WindowResize; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "WindowResize"; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
    private:
        int m_Width, m_Height;
    };

    // --- Key Events ---
    class KeyPressedEvent : public Event
    {
    public:
        KeyPressedEvent(unsigned int keycode, bool repeat)
            : m_KeyCode(keycode), m_Repeat(repeat) {}
        static EventType GetStaticType() { return EventType::KeyPressed; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "KeyPressed"; }
        unsigned int GetKeyCode() const { return m_KeyCode; }
        bool IsRepeat() const { return m_Repeat; }
    private:
        unsigned int m_KeyCode;
        bool m_Repeat;
    };

    class KeyReleasedEvent : public Event
    {
    public:
        explicit KeyReleasedEvent(unsigned int keycode)
            : m_KeyCode(keycode) {}
        static EventType GetStaticType() { return EventType::KeyReleased; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "KeyReleased"; }
        unsigned int GetKeyCode() const { return m_KeyCode; }
    private:
        unsigned int m_KeyCode;
    };

    // --- Mouse Button Events ---
    class MouseButtonPressedEvent : public Event
    {
    public:
        explicit MouseButtonPressedEvent(uint8_t button)
            : m_Button(button) {}
        static EventType GetStaticType() { return EventType::MouseButtonPressed; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "MouseButtonPressed"; }
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    class MouseButtonReleasedEvent : public Event
    {
    public:
        explicit MouseButtonReleasedEvent(uint8_t button)
            : m_Button(button) {}
        static EventType GetStaticType() { return EventType::MouseButtonReleased; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "MouseButtonReleased"; }
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    // --- Mouse Motion ---
    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_X(x), m_Y(y) {}
        static EventType GetStaticType() { return EventType::MouseMoved; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "MouseMoved"; }
        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
    private:
        float m_X, m_Y;
    };

    // --- Mouse Scroll ---
    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float offsetX, float offsetY)
            : m_OffsetX(offsetX), m_OffsetY(offsetY) {}
        static EventType GetStaticType() { return EventType::MouseScrolled; }
        EventType GetEventType() const override { return GetStaticType(); }
        const char* GetName() const override { return "MouseScrolled"; }
        float GetOffsetX() const { return m_OffsetX; }
        float GetOffsetY() const { return m_OffsetY; }
    private:
        float m_OffsetX, m_OffsetY;
    };

    // --- Factory ---
    Scope<Event> CreateEventFromSDL(const SDL_Event& sdlEvent);

    // --- Dispatcher ---
    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& event) : m_Event(event) {}

        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;
    };
}

#endif //AZER_EVENT_H
