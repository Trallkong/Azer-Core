//
// Created by csis on 2026/9/29.
//

#pragma once

#include "Base.h"
#include "Node2D.h"
#include "Type.h"

namespace Azer
{
    enum class ButtonState : uint8_t
    {
        NORMAL = 0,
        HOVER = 1,
        ACTIVE = 2
    };

    class Button : public Node2D
    {
    public:
        explicit Button(std::string name)
            : Node2D(std::move(name)) {}
        ~Button() override = default;

        void PhysicsProcess(float delta) override;
        void Draw() override;
        void OnEvent(Event& event) override;

    private:
        Color m_NormalColor = { .r = 100, .g = 100, .b = 100, .a = 255 };
        Color m_HoverColor = { .r = 20, .g = 80, .b = 200, .a = 255 };
        Color m_ActiveColor = { .r = 200, .g = 80, .b = 100, .a = 255 };

        ButtonState m_ButtonState = ButtonState::NORMAL;

        [[nodiscard]] bool CursorInBounds(Vector2 cursor_pos) const;
    };
}



