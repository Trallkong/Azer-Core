//
// Created by Trallkong on 2026/5/5.
//

#pragma once

#include "Base.h"
#include "Window.h"

#include "SDL3/SDL.h"

namespace Azer
{
    class SDL3Window : public Window
    {
    public:
        explicit SDL3Window(uint32_t width, uint32_t height, const std::string& title);
        ~SDL3Window() override;

        // Setter
        void Resize(uint32_t width, uint32_t height) override;
        void SetTitle(const std::string& title) override;
        void SetResizable(bool resizable) override;
        void SetWindowIcon(const std::string& path) override;

        // Getter
        void* GetHandle() const override { return m_Window; }
        WindowSize GetWindowSize() const override;


    private:
        SDL_Window* m_Window;
    };
} // azer
