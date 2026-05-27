//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_INPUT_H
#define AZER_DEV_INPUT_H

#include <unordered_set>

#include "Base.h"

namespace azer
{
    class Input
    {
    public:
        static bool IsKeyPressed(const SDL_Keycode key)
        {
            if (s_Instance->m_PressedKeys.contains(key)) return true;
            return false;
        }

        static void KeyPressed(const SDL_Keycode key)
        {
            s_Instance->m_PressedKeys.insert(key);
        }

        static void KeyReleased(const SDL_Keycode key)
        {
            s_Instance->m_PressedKeys.erase(key);
        }

    private:
        std::unordered_set<SDL_Keycode> m_PressedKeys;
        static Scope<Input> s_Instance;
    };
} // azer

#endif //AZER_DEV_INPUT_H
