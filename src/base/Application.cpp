#include "azpch.h"
#include "Application.h"

#include <ranges>

#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

#include "imgui.h"
#include "Logger.h"

#include "SplashLayer.h"

#include "FileSystem.h"
#include "WindowEvent.h"

#include "GLFW/glfw3.h"

namespace Azer
{
    void Application::PushEvent(Scope<Event> event)
    {
        m_EventQueue.push(std::move(event));
    }

    Scope<Event> Application::PopEvent()
    {
        Scope<Event> event = std::move(m_EventQueue.front());
         m_EventQueue.pop();
        return event;
    }

    Application* Application::s_Instance = nullptr;

    Application::Application(
        const std::string& windowTitle)
    {
        s_Instance = this;

        // 资源根目录由 CMake 传入（Azer-Core/），assets/shaders 等都在它下面。
        // 不要在这里硬编码绝对路径：换机器/换仓库位置后 shader 会全部读不到。
        FileSystem::Init(AZER_ASSET_ROOT);

        m_Window = Window::Create(1280, 720, m_WindowTitle);

        m_Renderer = Renderer::Create();

        if (!m_Renderer->Initialize(m_Window.get()))
        {
            AZ_CORE_ERROR("Failed to initialize renderer");
            assert(false);
        }

        RenderCommand::Init(m_Renderer.get());
        Renderer2D::Init();
        Renderer3D::Init();

        m_ImGuiLayer = new ImGuiLayer(m_Renderer.get());
        PushLayer(m_ImGuiLayer);
    }

    Application::~Application()
    {
        for (const auto & i : std::views::reverse(m_LayerStack))
        {
            i->OnDetach();
            delete i;
        }

        Renderer2D::Shutdown();
        Renderer3D::Shutdown();
        m_Renderer->Shutdown();
        m_Renderer.reset();

        m_Window.reset();
    }

    void Application::Run()
    {
        while (m_Running)
        {
            const auto& layers = m_LayerStack.GetLayers();

            glfwPollEvents();

            while (!m_EventQueue.empty())
            {
                const Scope<Event>& event = PopEvent();
                OnEvent(*event.get());
                for (const auto layer : layers)
                {
                    layer->OnEvent(*event.get());
                }
            }

            // OnUpdate
            const float dt = m_DeltaTime.GetDeltaTime();

            // 固定时间步长（物理/确定性更新）
            m_Accumulator += dt;
            while (m_Accumulator >= m_FixedTimestep)
            {
                m_Accumulator -= m_FixedTimestep;
                for (const auto layer : layers)
                {
                    layer->OnPhysicsUpdate(m_FixedTimestep);
                }
            }
            // 防止螺旋死亡（掉帧太多时直接重置）
            if (m_Accumulator > m_FixedTimestep * 3.0f)
                m_Accumulator = 0.0f;

            // 可变帧率更新（输入、相机等）
            for (const auto layer : layers)
            {
                layer->OnUpdate(dt);
            }


            // 物理插值（alpha = 当前帧在两次物理 tick 间的进度）
            const float alpha = m_FixedTimestep > 0.0f
                                    ? glm::clamp(m_Accumulator / m_FixedTimestep, 0.0f, 1.0f)
                                    : 1.0f;
            for (const auto layer : layers)
            {
                layer->OnInterpolate(alpha);
            }

            if (!m_Minimized)
            {
                // OnDraw
                m_ImGuiLayer->Begin();
                OnImGuiRender();
                for (const auto layer : layers)
                {
                    layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();

                m_Renderer->BeginFrame(glm::vec3(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]));
                for (const auto layer : layers)
                {
                    layer->OnDraw();
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
            }

            // 垃圾回收
            for (const auto* layer : m_LayersToDelete)
            {
                delete layer;
            }
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

    void Application::OnEvent(const Event& e)
    {
        if (e.GetEventType() == EventType::WindowCloseEvent)
        {
            m_Running = false;
        }

        if (e.GetEventType() == EventType::WindowResizeEvent)
        {
            OnWindowResize(dynamic_cast<const WindowResizeEvent&>(e));
        }
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
        m_Renderer->Resize(event.GetWidth(), event.GetHeight());
        m_Minimized = false;
        return false;
    }

    bool Application::OnWindowMinimized(const WindowMinimizedEvent &event)
    {
        AZ_CORE_DEBUG("Window Minimized!");
        m_Minimized = true;
        return false;
    }
}
