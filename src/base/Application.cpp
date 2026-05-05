//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "Application.h"

#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "Logger.h"
#include "RenderSettings.h"
#include "SplashLayer.h"
#include "SDL3GPURenderer.h"
#include "SDL3Renderer.h"

namespace azer
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const AppMode& mode, const std::string& windowTitle)
        :m_Mode(mode), m_WindowTitle(windowTitle)
    {
        s_Instance = this;

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            assert(false);
        }

        m_Window = Window::Create(1280, 720, m_WindowTitle);

        if (mode == AppMode::Simple2D)
        {
            m_Renderer = CreateScope<SDL3Renderer>();
        } else if (mode == AppMode::ForwardPlus)
        {
            m_Renderer = CreateScope<SDL3GPURenderer>();
        } else
        {
            assert(false && "Unsupported AppMode");
        }

        if (!m_Renderer->Initialize(m_Window.get()))
        {
            AZ_CORE_ERROR("Failed to initialize renderer");
            assert(false);
        }


        m_ImGuiLayer = new ImGuiLayer();
        PushLayer(m_ImGuiLayer);
        PushOverlay(new SplashLayer(5));
    }

    Application::~Application()
    {
        for (auto i = m_LayerStack.rbegin(); i != m_LayerStack.rend(); ++i)
        {
            (*i)->OnDetach();
            delete *i;
        }

        if (m_Mode == AppMode::Simple2D)
        {
            SDL_DestroyRenderer(dynamic_cast<SDL3Renderer*>(m_Renderer.get())->GetRenderer());
            m_Window.reset();
        } else if (m_Mode == AppMode::ForwardPlus)
        {
            m_Renderer.reset();
        } else
        {
            assert(false && "Unsupported AppMode");
        }

        SDL_Quit();
    }

    void Application::Run()
    {
        while (m_Running)
        {
            std::vector<Layer*> layers = m_LayerStack.GetLayers();

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
                    if (event->Handled)
                        break;
                }
            }

            // OnUpdate
            const float dt = m_DeltaTime.GetDeltaTime();
            for (auto i = layers.begin(); i != layers.end(); ++i)
                (*i)->OnUpdate(dt);

            // OnDraw
            m_Renderer->BeginFrame(glm::vec3(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]));
            ImGuiLayer::Begin();
            OnImGuiRender();
            for (auto i = layers.begin(); i != layers.end(); ++i)
            {
                (*i)->OnDraw();
                (*i)->OnImGuiRender();
            }
            ImGuiLayer::End();
            m_Renderer->EndFrame();

            // 垃圾回收
            for (const auto* layer : m_LayersToDelete)
                delete layer;
            m_LayersToDelete.clear();
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    Layer* Application::PopLayer()
    {
        Layer* layer = m_LayerStack.PopLayer();
        layer->OnDetach();
        m_LayersToDelete.push_back(layer);
        return layer;
    }

    Layer* Application::PopOverlay()
    {
        Layer* layer = m_LayerStack.PopOverlay();
        layer->OnDetach();
        m_LayersToDelete.push_back(layer);
        return layer;
    }

    void Application::print_available_graphic_api()
    {
        const int n = SDL_GetNumRenderDrivers();
        std::cout << "Available graphic APIs: ";
        for (int i = 0; i < n; ++i)
        {
            const char* driver = SDL_GetRenderDriver(i);
            std::cout << driver << " ";
        }
        std::cout << std::endl;
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(OnWindowResize);
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
        RenderSettings::SetViewport(event.GetWidth(), event.GetHeight(), 0, 0);
        return false;
    }
}


