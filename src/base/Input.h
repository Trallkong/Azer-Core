//
// Created by Trallkong on 2026/5/5.
//

#pragma once
#include <unordered_set>

#include "Base.h"
#include "Type.h"

namespace Azer
{
    class Input
    {
    public:
        static bool IsKeyPressed(const int key)
        {
            if (s_Instance->m_PressedKeys.contains(key)) return true;
            return false;
        }

        static void KeyPressed(const int key)
        {
            s_Instance->m_PressedKeys.insert(key);
        }

        static void KeyReleased(const int key)
        {
            s_Instance->m_PressedKeys.erase(key);
        }

        static void MouseMoved(const float x, const float y)
        {
            s_Instance->m_LastMousePosition.x = x;
            s_Instance->m_LastMousePosition.y = y;
        }

        static Vector2 GetMousePosition()
        {
            return s_Instance->m_LastMousePosition;
        }

    private:
        std::unordered_set<int> m_PressedKeys;
        Vector2 m_LastMousePosition;
        static Scope<Input> s_Instance;
    };
} // azer

