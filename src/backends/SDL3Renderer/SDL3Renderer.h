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
        void SetCamera(Camera& camera) override;
        void ResetRenderState() override;
        void SetRenderTarget(Framebuffer* target) override;
        void Resize(uint32_t width, uint32_t height) override {}
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override {}


        // Renderer2D
        void DrawQuad(const Transform2D& transform, float alpha) override;
        void DrawColorQuad(const Transform2D& transform, const glm::vec4& color) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const Transform2D& transform, float alpha) override;
        Ref<Texture> CreateTexture(const std::string& filePath) override;
        Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;
        Ref<Texture> CreateHDRTexture(const std::string& filePath) override;
        Ref<Framebuffer> CreateFramebuffer(const FramebufferSpec& spec) override;

        // ImGui
        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        SDL_Renderer* GetRenderer() const { return m_Renderer; }

        // No Implement
        void DrawCube(const Transform3D& transform) override { assert(false); }
        void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha) override { assert(false); }
        void DrawSkybox(const Ref<Texture>& hdrTexture) override { assert(false); }

    private:
        SDL_Renderer* m_Renderer = nullptr;
        float offsetX = 0.0f, offsetY = 0.0f;
        float zoom = 1.0f;
    };
}

