#pragma once

#include "Base.h"
#include "Renderer.h"

#include "VulkanRendererContext.h"

namespace Azer {

    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer() = default;
        virtual ~VulkanRenderer() override;

        bool Initialize(Window* window) override;
        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void SetCamera(Camera& camera) override;
        void ResetRenderState() override;
        void SetRenderTarget(Framebuffer* target) override;  // nullptr = swapchain
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override;

        void DrawQuad(float x, float y, float w, float h, float alpha = 1.0f) override;
        void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color, float alpha = 1.0f) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle = 0.0f, float alpha = 1.0f) override;

        void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) override;
        void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha = 1.0f) override;
        void DrawSkybox(const Ref<Texture>& hdrTexture) override;

        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;

        void SetImGuiDrawData(ImDrawData* drawData) override;
        Ref<Texture> CreateTexture(const std::string& filePath) override;
        Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;
        Ref<Texture> CreateHDRTexture(const std::string& filePath) override;
        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

    private:
        VulkanRendererContext m_Context;
    };
}