//
// Created by Trallkong on 2026/9/27.
//

#include "WindowsWindow.h"

#include "Application.h"
#include "Input.h"
#include "WindowEvent.h"
#include "MouseEvent.h"
#include "KeyEvent.h"

namespace Azer {
    WindowsWindow::WindowsWindow(const uint32_t width, const uint32_t height, const char* title)
    {
        if (!glfwInit())
            AZ_ASSERT(false, "Failed to initialize GLFW");

        if (!glfwVulkanSupported())
            AZ_ASSERT(false, "GLFW is not supported");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);

        if (!m_Window)
        {
            glfwTerminate();
            AZ_ASSERT(false, "Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_Window);

        // 记录初始帧缓冲尺寸作为兜底值。构造参数不一定等于真实客户区
        // （DPI 缩放、系统边框等），而这个值会驱动 Vulkan 的 viewport 和
        // 交换链重建，所以必须是确定的数值。
        int frameBufferWidth = 0;
        int frameBufferHeight = 0;
        glfwGetFramebufferSize(m_Window, &frameBufferWidth, &frameBufferHeight);
        m_Width = (frameBufferWidth > 0) ? static_cast<uint32_t>(frameBufferWidth) : width;
        m_Height = (frameBufferHeight > 0) ? static_cast<uint32_t>(frameBufferHeight) : height;

        Application& app = Application::Get();
        glfwSetWindowUserPointer(m_Window, &app);

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, const int action, int mods) {
            auto* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (!application) return;

            if (action == GLFW_PRESS)
            {
                Input::KeyPressed(key);
                application->PushEvent(CreateScope<KeyPressedEvent>(key, false));
            }
            else if (action == GLFW_RELEASE)
            {
                Input::KeyReleased(key);
                application->PushEvent(CreateScope<KeyReleasedEvent>(key));
            }
            else if (action == GLFW_REPEAT)
            {
                Input::KeyPressed(key);
                application->PushEvent(CreateScope<KeyPressedEvent>(key, true));
            }
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            auto* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (!application) return;

            application->PushEvent(CreateScope<WindowCloseEvent>());
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
        {
            auto* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (!application) return;

            application->PushEvent(CreateScope<MouseMoveEvent>(static_cast<float>(x), static_cast<float>(y)));
            Input::MouseMoved(static_cast<float>(x), static_cast<float>(y));
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            auto* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (!application) return;

            if (action == GLFW_PRESS)
                application->PushEvent(CreateScope<MouseButtonPressedEvent>(button));
            else if (action == GLFW_RELEASE)
                application->PushEvent(CreateScope<MouseButtonReleasedEvent>(button));
        });
    }

    WindowSize WindowsWindow::GetWindowSize() const
    {
        if (m_Window == nullptr)
            return WindowSize(m_Width, m_Height);

        // 实时查询，避免窗口被拖动/缩放后返回过期尺寸。
        // 窗口最小化时 GLFW 返回 0x0，这是有效信息：调用方必须跳过
        // 交换链与视口更新，不能拿 0 去调 Vulkan。
        int frameBufferWidth = 0;
        int frameBufferHeight = 0;
        glfwGetFramebufferSize(m_Window, &frameBufferWidth, &frameBufferHeight);

        return WindowSize(
            static_cast<uint32_t>(frameBufferWidth > 0 ? frameBufferWidth : 0),
            static_cast<uint32_t>(frameBufferHeight > 0 ? frameBufferHeight : 0));
    }

    void WindowsWindow::Resize(const uint32_t width, const uint32_t height)
    {
        if (m_Window == nullptr || width == 0 || height == 0)
            return;

        glfwSetWindowSize(m_Window, static_cast<int>(width), static_cast<int>(height));
    }
} // Azer