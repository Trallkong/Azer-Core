#pragma once

#include "EventType.h"

namespace Azer {
    
    // --- Mouse Button Events ---
    class MouseButtonPressedEvent 
    {
    public:
        explicit MouseButtonPressedEvent(uint8_t button)
            : m_Button(button) {}
        static EventType GetStaticType() { return EventType::MouseButtonPressed; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "MouseButtonPressed"; }
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    class MouseButtonReleasedEvent 
    {
    public:
        explicit MouseButtonReleasedEvent(uint8_t button)
            : m_Button(button) {}
        static EventType GetStaticType() { return EventType::MouseButtonReleased; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "MouseButtonReleased"; }
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    // --- Mouse Motion ---
    class MouseMovedEvent 
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_X(x), m_Y(y) {}
        static EventType GetStaticType() { return EventType::MouseMoved; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "MouseMoved"; }
        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
    private:
        float m_X, m_Y;
    };

    // --- Mouse Scroll ---
    class MouseScrolledEvent 
    {
    public:
        MouseScrolledEvent(float offsetX, float offsetY)
            : m_OffsetX(offsetX), m_OffsetY(offsetY) {}
        static EventType GetStaticType() { return EventType::MouseScrolled; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "MouseScrolled"; }
        float GetOffsetX() const { return m_OffsetX; }
        float GetOffsetY() const { return m_OffsetY; }
    private:
        float m_OffsetX, m_OffsetY;
    };
}