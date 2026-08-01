#pragma once
#include "Base.h"
#include "Texture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Framebuffer.h"
#include "Window.h"

#include <vector>

#include "glm/glm.hpp"
#include "imgui.h"
#include "SDL3/SDL.h"

namespace Azer
{
    // 后端渲染器：只负责帧生命周期（acquire / pass / present）与低层绘制命令的执行。
    // 便利绘制（DrawQuad/DrawCube 等）由前端 Renderer2D / Renderer3D 负责，经 RenderCommand 提交到这里。
    class Renderer {
    public:
        virtual ~Renderer() = default;

        virtual bool Initialize(Window* window) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame(const glm::vec3& clearColor) = 0;
        virtual void EndFrame() = 0;
        virtual void ResetRenderState() = 0;
        virtual void SetRenderTarget(Framebuffer* target) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) = 0;

        virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                                 const Ref<IndexBuffer>& indexBuffer,
                                 const Ref<Shader>& shader) = 0;

        virtual void Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                          const Ref<Shader>& shader) = 0;

        // ImGui
        virtual void ImGuiInit(SDL_Window* window) = 0;
        virtual void ImGuiShutdown() = 0;
        virtual void ImGuiNewFrame() = 0;
        virtual void SetImGuiDrawData(ImDrawData* drawData) = 0;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) = 0;

        static Scope<Renderer> Create();
    };
}
