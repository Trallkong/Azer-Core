//
// Created by Trallkong on 2026/9/27.
//

#include "WindowsWindow.h"

#include "Application.h"
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

        Application& app = Application::Get();
        glfwSetWindowUserPointer(m_Window, &app);

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (!app) return;

            if (action == GLFW_PRESS) {
                KeyPressedEvent(key, false);
                app->
            }
        });
    }
} // Azer