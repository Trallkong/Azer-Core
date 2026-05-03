//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_APPLICATION_H
#define AZER_APPLICATION_H

#include "Base.h"
#include "DeltaTime.h"
#include "ImGuiLayer.h"
#include "SDL3/SDL.h"
#include "LayerStack.h"
#include "Renderer.h"

namespace azer
{
    /* App的模式，决定使用渲染器的类型
     * Simple2D: 使用SDL3的默认Renderer，只支持简单的2D图形绘制，适合制作简单应用
     * Forward+: 使用SDL3的GPU封装，支持复杂的3D/2D图形渲染，适合制作大型游戏或图形应用
     */
    enum class AppMode
    {
        Simple2D,
        ForwardPlus,
    };

    class Application {
    public:
        explicit Application(const AppMode& mode = AppMode::Simple2D, const std::string& windowTitle = "Azer");
        ~Application();

        void Run();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        Layer* PopLayer();
        Layer* PopOverlay();

        SDL_Window* GetWindow() const { return m_Window; }
        const AppMode& GetMode() const { return m_Mode; }
        Renderer* GetRenderer() const { return m_Renderer.get(); }

        void SetSettingShow(const bool show) { m_ShowSettings = show; }
        const std::string& GetWindowTitle() const { return m_WindowTitle; }

        static Application& Get() { return *s_Instance; }
    private:
        // 打印可用图形API
        static void print_available_graphic_api();

        void OnImGuiRender();

        AppMode m_Mode;
        SDL_Window* m_Window = nullptr;
        Scope<Renderer> m_Renderer = nullptr;

        bool m_Running = true;
        SDL_Event m_Event {};

        LayerStack m_LayerStack {};
        DeltaTime m_DeltaTime {};

        static Application* s_Instance;
        ImGuiLayer* m_ImGuiLayer = nullptr;

        float m_ClearColor[3] = { 0.0f, 0.0f, 0.0f};
        bool m_ShowSettings = true;
        std::string m_WindowTitle = "Azer";

        std::vector<Layer*> m_LayersToDelete;
    };

    Application* CreateApplication();
}

#endif //AZER_APPLICATION_H
