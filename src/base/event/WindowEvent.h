#pragma once

#include "EventType.h"

namespace Azer {
    
    // --- Window Events ---
    class WindowCloseEvent 
    {
    public:
        static EventType GetStaticType() { return EventType::WindowClose; }
        EventType GetEventType() const { return GetStaticType(); }
        const char* GetName() const { return "WindowClose"; }
    };

    class WindowResizeEvent 
    {
    public:
        WindowResizeEvent(int width, int height)
            : m_Width(width), m_Height(height) {}
        static EventType GetStaticType() { return EventType::WindowResize; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "WindowResize"; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
    private:
        int m_Width, m_Height;
    };

    class WindowMinimizedEvent 
    {
    public:
        WindowMinimizedEvent() = default;
        static EventType GetStaticType() { return EventType::WindowMinimized; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "WindowMinimized"; }
    };

    class WindowRestoredEvent
    {
    public:
        WindowRestoredEvent() = default;
        static EventType GetStaticType() { return EventType::WindowRestored; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "WindowRestored"; }
    };
}