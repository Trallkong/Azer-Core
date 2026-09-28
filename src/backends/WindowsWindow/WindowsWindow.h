//
// Created by Trallkong on 2026/9/27.
//
#pragma once

#include "Window.h"
#include "GLFW/glfw3.h"

namespace Azer {
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(uint32_t width, uint32_t height, const char* title);

        void Resize(uint32_t width, uint32_t height) override {};

        void SetTitle(const std::string &title) override {};

        void SetResizable(bool resizable) override {};

        void SetWindowIcon(const std::string &path) override {};

        void * GetHandle() const override { return m_Window; };

        WindowSize GetWindowSize() const override { return WindowSize(m_Width, m_Height); };

    private:
        GLFWwindow* m_Window;
        uint32_t m_Width;
        uint32_t m_Height;
    };
} // Azer


