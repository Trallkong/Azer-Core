//
// Created by Trallkong on 2026/4/18.
//

#pragma once
#include "Base.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Window.h"
#include "Camera.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include "SDL3/SDL.h"

namespace azer
{
    class Renderer {
    public:
        virtual ~Renderer() = default;
        virtual bool Initialize(Window* window) = 0;
        virtual void BeginFrame(const glm::vec3& clearColor) = 0;
        virtual void EndFrame() = 0;
        virtual void SetCamera(Camera& camera) = 0;
        virtual void ResetRenderState() = 0;  // 重置渲染状态（缩放等），在切换渲染目标后调用
        virtual void SetRenderTarget(Framebuffer* target) = 0;  // nullptr = swapchain
        virtual void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) = 0;

        // Renderer2D
        virtual void DrawQuad(float x, float y, float w, float h, float alpha = 1.0f) = 0;
        virtual void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color, float alpha = 1.0f) = 0;
        virtual void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle = 0.0f, float alpha = 1.0f) = 0;

        // Renderer3D
        virtual void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) = 0;
        virtual void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha = 1.0f) = 0;
        virtual void DrawSkybox(const Ref<Texture>& hdrTexture) = 0;

        // ImGui 生命周期（各后端自行初始化对应 ImGui_Impl_*）
        virtual void ImGuiInit(SDL_Window* window) = 0;
        virtual void ImGuiShutdown() = 0;
        virtual void ImGuiNewFrame() = 0;

        // 这一步本质上是把绘制数据传给GPU，发生在ImGui渲染End时，真正的提交在渲染器End时
        virtual void SetImGuiDrawData(ImDrawData* drawData) = 0;

        virtual Ref<Texture> CreateTexture(const std::string& filePath) = 0;
        virtual Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) = 0;
        virtual Ref<Texture> CreateHDRTexture(const std::string& filePath) = 0;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) = 0;

        static Scope<Renderer> CreateRendererFromAppMode(const AppMode& mode);
    };
}

