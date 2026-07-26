//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include "Base.h"
#include "DeltaTime.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer.h"
#include "SDL3/SDL.h"
#include "Window.h"

namespace Azer
{

    class Application {
    public:
        explicit Application(
            const std::string& rootPath,
            const std::string& windowTitle = "Azer");
        ~Application();

        void Run();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer();
        void PopOverlay();

        inline Window& GetWindow() const { return *m_Window.get(); }
        inline Renderer* GetRenderer() const { return m_Renderer.get(); }
        inline void SetCoreMenuVisibility(const bool show) { m_ShowSettings = show; }
        inline const std::string& GetWindowTitle() const { return m_WindowTitle; }
        inline void SetPhysicsHz(float hz) { m_PhysicsHz = hz; m_FixedTimestep = 1.0f / hz; }
        inline float GetPhysicsHz() const { return m_PhysicsHz; }

    private:
        void OnEvent(Event& e);
        void OnImGuiRender();
        bool OnWindowResize(const WindowResizeEvent& event);

        Scope<Window> m_Window = nullptr;
        Scope<Renderer> m_Renderer = nullptr;

        bool m_Running = true;
        SDL_Event m_Event {};

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

