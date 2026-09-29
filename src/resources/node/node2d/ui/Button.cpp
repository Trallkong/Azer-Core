//
// Created by csis on 2026/9/29.
//

#include "azpch.h"
#include "Button.h"

#include "Event.h"
#include "Input.h"
#include "Logger.h"
#include "MouseEvent.h"
#include "Renderer2D.h"

namespace Azer
{
    void Button::PhysicsProcess(float delta)
    {
        Node2D::PhysicsProcess(delta);

        switch (m_ButtonState)
        {
        case ButtonState::NORMAL:
            if (m_ButtonState != ButtonState::ACTIVE && CursorInBounds(Input::GetMousePosition()))
                m_ButtonState = ButtonState::HOVER;
            break;
        case ButtonState::HOVER:
            if (m_ButtonState != ButtonState::ACTIVE && !CursorInBounds(Input::GetMousePosition()))
                m_ButtonState = ButtonState::NORMAL;
            break;
        case ButtonState::ACTIVE:
            break;
        }
    }

    void Button::Draw()
    {
        Node2D::Draw();

        switch (m_ButtonState)
        {
        case ButtonState::NORMAL:
            Renderer2D::DrawColorQuad(Transform, m_NormalColor);
            break;
        case ButtonState::HOVER:
            Renderer2D::DrawColorQuad(Transform, m_HoverColor);
            break;
        case ButtonState::ACTIVE:
            Renderer2D::DrawColorQuad(Transform, m_ActiveColor);
            break;
        }
    }

    void Button::OnEvent(Event& event)
    {
        Node2D::OnEvent(event);

        switch (m_ButtonState)
        {
        case ButtonState::NORMAL:
            if (event.GetEventType() == EventType::MouseButtonPressed)
                m_ButtonState = ButtonState::ACTIVE;
            break;
        case ButtonState::HOVER:
            if (event.GetEventType() == EventType::MouseButtonPressed)
                m_ButtonState = ButtonState::ACTIVE;
            break;
        case ButtonState::ACTIVE:
            break;
        }
    }

    bool Button::CursorInBounds(const Vector2 cursor_pos) const
    {
        const float l = Transform.Position.x - Transform.Scale.x / 2.0;
        const float r = Transform.Position.x + Transform.Scale.x / 2.0;
        const float t = Transform.Position.y + Transform.Scale.y / 2.0;
        const float b = Transform.Position.y - Transform.Scale.y / 2.0;

        const float x = cursor_pos.x - 640;
        const float y = cursor_pos.y - 360;

        return x >= l && x <= r && y >= b && y <= t;
    }
}
