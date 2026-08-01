//
// Created by Trallkong on 2026/4/18.
//

#pragma once

#include "Base.h"
#include "Renderer.h"

namespace Azer
{
    class SDL3Renderer : public Renderer {
    public:
        SDL3Renderer() = default;
        ~SDL3Renderer() override = default;

        bool Initialize(Window* window) override;
        void Shutdown() override;
        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void ResetRenderState() override;
        void SetRenderTarget(Framebuffer* target) override;
        void Resize(uint32_t width, uint32_t height) override {}
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override {}

        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

        // ImGui
        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        static SDL_Renderer* GetRenderer() { return s_Renderer; }

        // 低层绘制（RenderCommand 使用）：当前后端不支持，占位
        void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                         const Ref<IndexBuffer>& indexBuffer,
                         const Ref<Shader>& shader) override { assert(false); }
        void Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                  const Ref<Shader>& shader) override { assert(false); }

    private:
        SDL_Renderer* m_Renderer = nullptr;
        static SDL_Renderer* s_Renderer;
        float offsetX = 0.0f, offsetY = 0.0f;
        float zoom = 1.0f;
    };
}

