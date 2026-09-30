//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include "Base.h"
#include <queue>

namespace Azer
{
    class Event;
    class WindowResizeEvent;
    class WindowMinimizedEvent;
    class ImGuiLayer;
    class LayerStack;
    class Layer;
    class Renderer;
    class Window;
    class DeltaTime;

    class Application {
    public:
        explicit Application(const std::string& windowTitle = "Azer");
        ~Application();

        void Run();

        void PushLayer(Layer* layer) const;
        void PushOverlay(Layer* overlay) const;
        void PopLayer();
        void PopOverlay();

        [[nodiscard]] Window& GetWindow() const { return *m_Window.get(); }
        [[nodiscard]] Renderer* GetRenderer() const { return m_Renderer.get(); }
        void SetCoreMenuVisibility(const bool show) { m_ShowSettings = show; }
        [[nodiscard]] const std::string& GetWindowTitle() const { return m_WindowTitle; }
        void SetPhysicsHz(float hz) { m_PhysicsHz = hz; m_FixedTimestep = 1.0f / hz; }
        [[nodiscard]] float GetPhysicsHz() const { return m_PhysicsHz; }

        // EventQueue
        void PushEvent(Scope<Event> event);
        Scope<Event> PopEvent();

        static Application& Get() { return *s_Instance; }
    private:
        static Application* s_Instance;

        void OnEvent(const Event& e);
        void OnImGuiRender();
        bool OnWindowResize(const WindowResizeEvent& event);

        Scope<Window> m_Window;
        Scope<Renderer> m_Renderer;

        bool m_Running = true;
        bool m_Minimized = false;
        std::queue<Scope<Event>> m_EventQueue;

        Scope<LayerStack> m_LayerStack;
        Scope<DeltaTime> m_DeltaTime;
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

