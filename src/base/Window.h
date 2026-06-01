//
// Created by Trallkong on 2026/4/18.
//

#pragma once

#include <string>

#include "Base.h"

namespace azer
{
    struct WindowSize
    {
        uint32_t width;
        uint32_t height;
    };

    class Window {
    public:
        virtual ~Window() = default;

        // Setter
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetResizable(bool resizable) = 0;
        virtual void SetWindowIcon(const std::string& path) = 0;

        // Getter
        virtual void* GetHandle() const = 0;
        virtual WindowSize GetWindowSize() const = 0;

        static Scope<Window> Create(uint32_t width, uint32_t height, const std::string& title);
    };
}
