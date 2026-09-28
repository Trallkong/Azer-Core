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

        void Resize(uint32_t width, uint32_t height) override;

        void SetTitle(const std::string &title) override {};

        void SetResizable(bool resizable) override {};

        void SetWindowIcon(const std::string &path) override {};

        void * GetHandle() const override { return m_Window; };

        WindowSize GetWindowSize() const override;

    private:
        GLFWwindow* m_Window = nullptr;
        // 仅作为 GLFW 窗口不可用时的兜底值。窗口尺寸统一在 GetWindowSize() 里
        // 实时查询，避免缓存值与真实帧缓冲尺寸脱节。
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };
} // Azer


