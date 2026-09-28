#pragma once

#include "Event.h"

namespace Azer {

    // --- Window Events ---
    class WindowCloseEvent : public EventBase<EventType::WindowCloseEvent, EventCategoryFlag::WindowEvent> { };

    class WindowResizeEvent : public EventBase<EventType::WindowResizeEvent, EventCategoryFlag::WindowEvent>
    {
    public:
        explicit WindowResizeEvent(const uint32_t width, const uint32_t height)
            : m_Width(width), m_Height(height) {}
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
    private:
        int m_Width, m_Height;
    };
}