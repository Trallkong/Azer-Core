//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_APPLICATION_H
#define AZER_APPLICATION_H

#include "Base.h"
#include "DeltaTime.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
#include "Renderer.h"
#include "SDL3/SDL.h"
#include "Window.h"

namespace azer
{

    class Application {
    public:
        explicit Application(const AppMode& mode = AppMode::Simple2D, const std::string& windowTitle = "Azer");
        ~Application();

        void Run();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer();
        void PopOverlay();

        Window& GetWindow() const { return *m_Window.get(); }
        const AppMode& GetMode() const { return m_Mode; }
        Renderer* GetRenderer() const { return m_Renderer.get(); }

        void SetSettingShow(const bool show) { m_ShowSettings = show; }
        const std::string& GetWindowTitle() const { return m_WindowTitle; }
        void SetPhysicsHz(float hz) { m_PhysicsHz = hz; m_FixedTimestep = 1.0f / hz; }
        float GetPhysicsHz() const { return m_PhysicsHz; }
    private:
        void OnEvent(Event& e);
        void OnImGuiRender();
        bool OnWindowResize(const WindowResizeEvent& event);

        AppMode m_Mode;
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

#endif //AZER_APPLICATION_H
