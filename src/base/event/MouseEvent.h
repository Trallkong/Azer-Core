#pragma once

#include "Event.h"

namespace Azer {
    
    // --- Mouse Button Events ---
    class MouseButtonPressedEvent : public EventBase<EventType::MouseButtonPressed, EventCategoryFlag::MouseEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit MouseButtonPressedEvent(const uint8_t button)
            : m_Button(button) {}
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    class MouseButtonReleasedEvent : public EventBase<EventType::MouseButtonReleased, EventCategoryFlag::MouseEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit MouseButtonReleasedEvent(uint8_t button)
            : m_Button(button) {}
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    class MouseMoveEvent : public EventBase<EventType::MouseMoved, EventCategoryFlag::MouseEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit MouseMoveEvent(const float x, const float y)
            : m_X(x), m_Y(y) {}

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }
    private:
        float m_X, m_Y;
    };
}