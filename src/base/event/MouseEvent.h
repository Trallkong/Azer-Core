#pragma once

#include "Event.h"

namespace Azer {
    
    // --- Mouse Button Events ---
    class MouseButtonPressedEvent : public EventBase<EventType::MouseButtonPressedEvent, EventCategoryFlag::MouseEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit MouseButtonPressedEvent(const uint8_t button)
            : m_Button(button) {}
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };

    class MouseButtonReleasedEvent : public EventBase<EventType::MouseButtonReleasedEvent, EventCategoryFlag::MouseEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit MouseButtonReleasedEvent(uint8_t button)
            : m_Button(button) {}
        uint8_t GetButton() const { return m_Button; }
    private:
        uint8_t m_Button;
    };
}