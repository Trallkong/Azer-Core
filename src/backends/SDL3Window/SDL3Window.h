//
// Created by Trallkong on 2026/5/5.
//

#ifndef AZER_DEV_SDL3WINDOW_H
#define AZER_DEV_SDL3WINDOW_H

#include "Base.h"
#include "Window.h"

#include "SDL3/SDL.h"

namespace azer
{
    class SDL3Window : public Window
    {
    public:
        explicit SDL3Window(uint32_t width, uint32_t height, const std::string& title);
        ~SDL3Window() override;

        void* GetHandle() const override { return m_Window; }
        void Resize(uint32_t width, uint32_t height) override;
        void SetTitle(const std::string& title) override;
        void SetResizable(bool resizable) override;

    private:
        SDL_Window* m_Window;
    };
} // azer

#endif //AZER_DEV_SDL3WINDOW_H
