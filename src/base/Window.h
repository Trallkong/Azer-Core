//
// Created by Trallkong on 2026/4/18.
//

#ifndef LEARNSDL_WINDOW_H
#define LEARNSDL_WINDOW_H

#include <string>

#include "Base.h"

namespace azer
{
    class Window {
    public:
        virtual ~Window() = default;

        virtual void* GetHandle() const = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetResizable(bool resizable) = 0;

        static Scope<Window> Create(uint32_t width, uint32_t height, const std::string& title);
    };
}





#endif //LEARNSDL_WINDOW_H
