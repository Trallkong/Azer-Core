//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "Window.h"

#include "SDL3Window.h"

namespace azer
{
    Scope<Window> Window::Create(uint32_t width, uint32_t height, const std::string& title)
    {
        return CreateScope<SDL3Window>(width, height, title);
    }
}

