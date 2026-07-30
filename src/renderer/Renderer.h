#pragma once
#include "Base.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Window.h"
#include "Camera.h"
#include "Model.h"
#include "Transform2D.h"
#include "Transform3D.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include "SDL3/SDL.h"

namespace Azer
{
    class Renderer {
    public:
        virtual ~Renderer() = default;

        virtual bool Initialize(Window* window) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame(const glm::vec3& clearColor) = 0;
        virtual void EndFrame() = 0;
        virtual void SetCamera(Camera& camera) = 0;
        virtual void ResetRenderState() = 0;
        virtual void SetRenderTarget(Framebuffer* target) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) = 0;

        // Renderer2D
        virtual void DrawQuad(const Transform2D& transform, float alpha = 1.0f) = 0;
        virtual void DrawColorQuad(const Transform2D& transform, const glm::vec4& color) = 0;
        virtual void DrawTexture(Texture* tex, const SDL_FRect& src, const Transform2D& transform, float alpha = 1.0f) = 0;

        // Renderer3D
        virtual void DrawCube(const Transform3D& transform) = 0;
        virtual void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha = 1.0f) = 0;
        virtual void DrawSkybox(const Ref<Texture>& hdrTexture) = 0;

        // ImGui
        virtual void ImGuiInit(SDL_Window* window) = 0;
        virtual void ImGuiShutdown() = 0;
        virtual void ImGuiNewFrame() = 0;
        virtual void SetImGuiDrawData(ImDrawData* drawData) = 0;

        virtual Ref<Texture> CreateTexture(const std::string& filePath) = 0;
        virtual Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) = 0;
        virtual Ref<Texture> CreateHDRTexture(const std::string& filePath) = 0;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) = 0;

        static Scope<Renderer> Create();
    };
}
