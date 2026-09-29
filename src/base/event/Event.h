//
// Created by Trallkong on 2026/5/1.
//

#pragma once
#include "Base.h"
#include "Type.h"

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

    // enum class has no built-in bitwise operators, so define them here.
    constexpr EventCategoryFlag operator|(const EventCategoryFlag lhs, const EventCategoryFlag rhs) noexcept
    {
        return static_cast<EventCategoryFlag>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
    }

    constexpr EventCategoryFlag operator&(const EventCategoryFlag lhs, const EventCategoryFlag rhs) noexcept
    {
        return static_cast<EventCategoryFlag>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
    }

    constexpr EventCategoryFlag& operator|=(EventCategoryFlag& lhs, const EventCategoryFlag rhs) noexcept
    {
        return lhs = (lhs | rhs);
    }

    constexpr EventCategoryFlag& operator&=(EventCategoryFlag& lhs, const EventCategoryFlag rhs) noexcept
    {
        return lhs = (lhs & rhs);
    }

    constexpr bool HasFlag(const EventCategoryFlag value, const EventCategoryFlag flag) noexcept
    {
        return (value & flag) != EventCategoryFlag::None;
    }

    enum class EventType
    {
        WindowClose, WindowResize, WindowFocus, WindowLostFocus,
        KeyPressed, KeyReleased,
        MouseMoved, MouseButtonPressed, MouseButtonReleased,
    };


    class Event
    {
    public:
        // Declaring the deleted copy ctor below suppresses the implicit default
        // ctor, which would leave EventBase<> (and every event) non-default-
        // constructible. Declare it explicitly.
        Event() = default;
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
}

