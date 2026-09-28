//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "Window.h"

#include "WindowsWindow.h"

namespace Azer
{
    Scope<Window> Window::Create(uint32_t width, uint32_t height, const std::string& title)
    {
        return CreateScope<WindowsWindow>(width, height, title.c_str());
    }
}

