//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include "Base.h"
#include "DeltaTime.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer.h"
#include "Window.h"

#include <queue>

namespace Azer
{
    class Event;
    class WindowResizeEvent;
    class WindowMinimizedEvent;

    class Application {
    public:
        explicit Application(const std::string& windowTitle = "Azer");
        ~Application();

        void Run();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer();
        void PopOverlay();

        Window& GetWindow() const { return *m_Window.get(); }
        Renderer* GetRenderer() const { return m_Renderer.get(); }
        void SetCoreMenuVisibility(const bool show) { m_ShowSettings = show; }
        const std::string& GetWindowTitle() const { return m_WindowTitle; }
        void SetPhysicsHz(float hz) { m_PhysicsHz = hz; m_FixedTimestep = 1.0f / hz; }
        float GetPhysicsHz() const { return m_PhysicsHz; }

        // EventQueue
        void PushEvent(Scope<Event> event);
        Scope<Event> PopEvent();

        static Application& Get() { return *s_Instance; }
    private:
        static Application* s_Instance;

        void OnEvent(const Event& e);
        void OnImGuiRender();
        bool OnWindowResize(const WindowResizeEvent& event);
        bool OnWindowMinimized(const WindowMinimizedEvent& event);

        Scope<Window> m_Window = nullptr;
        Scope<Renderer> m_Renderer = nullptr;

        bool m_Running = true;
        bool m_Minimized = false;
        std::queue<Scope<Event>> m_EventQueue;

        LayerStack m_LayerStack {};
        DeltaTime m_DeltaTime {};
        float m_Accumulator = 0.0f;
        float m_FixedTimestep = 1.0f / 60.0f;
        float m_PhysicsHz = 60.0f;

        ImGuiLayer* m_ImGuiLayer = nullptr;

        float m_ClearColor[3] = { 0.0f, 0.0f, 0.0f};
        bool m_ShowSettings = true;
        std::string m_WindowTitle = "Azer";

        std::vector<Layer*> m_LayersToDelete;
    };

    Application* CreateApplication();
}

