//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"

namespace Azer
{
    enum class EventCategoryFlag : unsigned
    {
        None            = 0,
        WindowEvent     = 1u << 0,
        KeyboardEvent   = 1u << 1,
        MouseEvent      = 1u << 2,
        InputEvent      = 1u << 3,
    };

    enum class EventType
    {
        WindowCloseEvent, WindowResizeEvent, WindowFocusEvent, WindowLostFocusEvent,
        KeyPressedEvent, KeyReleasedEvent,
        MouseMovedEvent, MouseButtonPressedEvent, MouseButtonReleasedEvent,
    };


    class Event
    {
    public:
        virtual ~Event() = default;

        Event(const Event& event) = delete;
        Event& operator=(const Event& event) = delete;

        void SetHandled(const bool handled) { m_Handled = handled; }
        virtual EventType GetEventType() const = 0;
        virtual EventCategoryFlag GetCategory() const = 0;
    private:
        bool m_Handled = false;
    };

    template<EventType Type, EventCategoryFlag Category>
    class EventBase : public Event
    {
    public:
        static constexpr EventType GetStaticType() { return Type; }

        EventType GetEventType() const noexcept override { return Type; }

        EventCategoryFlag GetCategory() const noexcept override { return Category; }
    };


    class EventBus
    {
    public:
        
    };
}

