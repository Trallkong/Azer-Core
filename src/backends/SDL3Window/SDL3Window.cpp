//
// Created by Trallkong on 2026/5/5.
//

#include "SDL3Window.h"

namespace azer
{
    SDL3Window::SDL3Window(const uint32_t width, const uint32_t height, const std::string& title)
    {
        m_Window = SDL_CreateWindow(
            title.c_str(),
            static_cast<int>(width),
            static_cast<int>(height),
            SDL_WINDOW_RESIZABLE
        );
    }

    SDL3Window::~SDL3Window()
    {
        if (m_Window)
            SDL_DestroyWindow(m_Window);
    }

    void SDL3Window::Resize(const uint32_t width, const uint32_t height)
    {
        SDL_SetWindowSize(m_Window, static_cast<int>(width), static_cast<int>(height));
    }

    void SDL3Window::SetTitle(const std::string& title)
    {
        SDL_SetWindowTitle(m_Window, title.c_str());
    }

    void SDL3Window::SetResizable(const bool resizable)
    {
        SDL_SetWindowResizable(m_Window, resizable);
    }
} // azer