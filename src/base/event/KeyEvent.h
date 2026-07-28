#pragma once

#include "EventType.h"

namespace Azer {
    
    // --- Key Events ---
    class KeyPressedEvent 
    {
    public:
        KeyPressedEvent(unsigned int keycode, bool repeat)
            : m_KeyCode(keycode), m_Repeat(repeat) {}
        static EventType GetStaticType() { return EventType::KeyPressed; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "KeyPressed"; }
        unsigned int GetKeyCode() const { return m_KeyCode; }
        bool IsRepeat() const { return m_Repeat; }
    private:
        unsigned int m_KeyCode;
        bool m_Repeat;
    };

    class KeyReleasedEvent 
    {
    public:
        explicit KeyReleasedEvent(unsigned int keycode)
            : m_KeyCode(keycode) {}
        static EventType GetStaticType() { return EventType::KeyReleased; }
        EventType GetEventType() const  { return GetStaticType(); }
        const char* GetName() const  { return "KeyReleased"; }
        unsigned int GetKeyCode() const { return m_KeyCode; }
    private:
        unsigned int m_KeyCode;
    };
}