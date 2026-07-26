#include "azpch.h"
#include "Application.h"

#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "Input.h"
#include "Logger.h"
#include "SDL3GPURenderer.h"

#include "SplashLayer.h"

#include "FileSystem.h"

namespace Azer
{
    Application::Application(
        const std::string& rootPath,
        const std::string& windowTitle)
        :m_WindowTitle(windowTitle)
    {
        FileSystem::Init(rootPath);

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            assert(false);
        }

        m_Window = Window::Create(1280, 720, m_WindowTitle);

        m_Renderer = Renderer::Create();

        if (!m_Renderer->Initialize(m_Window.get()))
        {
            AZ_CORE_ERROR("Failed to initialize renderer");
            assert(false);
        }

        m_ImGuiLayer = new ImGuiLayer(m_Renderer.get());
        PushLayer(m_ImGuiLayer);

        // PushOverlay(new SplashLayer(*m_Renderer.get(),5));
    }

    Application::~Application()
    {
        for (auto i = m_LayerStack.rbegin(); i != m_LayerStack.rend(); ++i)
        {
            (*i)->OnDetach();
            delete *i;
        }

        m_Renderer->Shutdown();
        m_Renderer.reset();

        m_Window.reset();

        SDL_Quit();
    }

    void Application::Run()
    {
        while (m_Running)
        {
            const auto& layers = m_LayerStack.GetLayers();

            while (SDL_PollEvent((&m_Event)))
            {
                ImGui_ImplSDL3_ProcessEvent(&m_Event);

                if (m_Event.type == SDL_EVENT_QUIT)
                    m_Running = false;

                Scope<Event> event = CreateEventFromSDL(m_Event);
                if (!event)
                    continue;

                OnEvent(*event);
                for (auto i = layers.rbegin(); i != layers.rend(); ++i)
                {
                    (*i)->OnEvent(*event);
                    if (event->IsHandled())
                        break;
                }
            }

            // OnUpdate
            const float dt = m_DeltaTime.GetDeltaTime();

            // 固定时间步长（物理/确定性更新）
            m_Accumulator += dt;
            while (m_Accumulator >= m_FixedTimestep)
            {
                m_Accumulator -= m_FixedTimestep;
                for (auto i = layers.begin(); i != layers.end(); ++i)
                    (*i)->OnPhysicsUpdate(m_FixedTimestep);
            }
            // 防止螺旋死亡（掉帧太多时直接重置）
            if (m_Accumulator > m_FixedTimestep * 3.0f)
                m_Accumulator = 0.0f;

            // 可变帧率更新（输入、相机等）
            for (auto i = layers.begin(); i != layers.end(); ++i)
                (*i)->OnUpdate(dt);

            // 物理插值（alpha = 当前帧在两次物理 tick 间的进度）
            const float alpha = m_FixedTimestep > 0.0f
                                    ? glm::clamp(m_Accumulator / m_FixedTimestep, 0.0f, 1.0f)
                                    : 1.0f;
            for (auto i = layers.begin(); i != layers.end(); ++i)
                (*i)->OnInterpolate(alpha);

            // OnDraw
            m_ImGuiLayer->Begin();
            OnImGuiRender();
            for (auto i = layers.begin(); i != layers.end(); ++i)
            {
                (*i)->OnImGuiRender();
            }
            m_ImGuiLayer->End();

            m_Renderer->BeginFrame(glm::vec3(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]));
            for (auto i = layers.begin(); i != layers.end(); ++i)
            {
                (*i)->OnDraw();
            }
            m_Renderer->EndFrame();

            // 移除标记为待删的层
            std::vector<Layer*> toDetach;
            for (auto* layer : m_LayerStack)
                if (layer->IsPendingRemove())
                    toDetach.push_back(layer);
            for (auto* layer : toDetach)
            {
                layer->OnDetach();
                m_LayerStack.Erase(layer);
                m_LayersToDelete.push_back(layer);
            }

            // 垃圾回收
            for (const auto* layer : m_LayersToDelete)
                delete layer;
            m_LayersToDelete.clear();
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        EngineContext ctx{*m_Renderer, *m_Window};
        layer->OnAttach(ctx);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        EngineContext ctx{*m_Renderer, *m_Window};
        overlay->OnAttach(ctx);
    }

    void Application::PopLayer()
    {
        Layer* layer = m_LayerStack.PeekLayer();
        layer->OnDetach();
        m_LayersToDelete.push_back(layer);
        m_LayerStack.PopLayer();
    }

    void Application::PopOverlay()
    {
        Layer* layer = m_LayerStack.PeekOverlay();
        layer->OnDetach();
        m_LayersToDelete.push_back(layer);
        m_LayerStack.PopOverlay();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e)
        {
            return OnWindowResize(e);
        });
        dispatcher.Dispatch<KeyPressedEvent>([](const KeyPressedEvent& e)
        {
            Input::KeyPressed(e.GetKeyCode());
            return false;
        });
        dispatcher.Dispatch<KeyReleasedEvent>([](const KeyReleasedEvent& e)
        {
            Input::KeyReleased(e.GetKeyCode());
            return false;
        });
    }

    void Application::OnImGuiRender()
    {
        if (!m_ShowSettings) return;
        ImGui::Begin("Azer Settings");
        ImGui::ColorEdit3("Clear Color", m_ClearColor);
        ImGui::End();
    }

    bool Application::OnWindowResize(const WindowResizeEvent& event)
    {
        AZ_CORE_TRACE("Window Resize Event: {0} {1}", event.GetWidth(), event.GetHeight());
        m_Renderer->SetViewport(event.GetWidth(), event.GetHeight(), 0, 0);
        return false;
    }
}
