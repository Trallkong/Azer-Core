#pragma once

#include "Event.h"

namespace Azer {
    
    // --- Key Events ---
    class KeyPressedEvent : public EventBase<EventType::KeyPressed, EventCategoryFlag::KeyboardEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit KeyPressedEvent(const unsigned int keycode, const bool repeat)
            : m_KeyCode(keycode), m_Repeat(repeat) {}

        unsigned int GetKeyCode() const { return m_KeyCode; }
        bool IsRepeat() const { return m_Repeat; }
    private:
        unsigned int m_KeyCode;
        bool m_Repeat;
    };

    class KeyReleasedEvent : public EventBase<EventType::KeyReleased, EventCategoryFlag::KeyboardEvent | EventCategoryFlag::InputEvent>
    {
    public:
        explicit KeyReleasedEvent(unsigned int keycode)
            : m_KeyCode(keycode) {}

        unsigned int GetKeyCode() const { return m_KeyCode; }
    private:
        unsigned int m_KeyCode;
    };
}