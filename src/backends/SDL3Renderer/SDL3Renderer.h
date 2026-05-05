//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_SDL3RENDERER_H
#define AZER_SDL3RENDERER_H

#include "Base.h"
#include "Renderer.h"

namespace azer
{
    class SDL3Renderer : public Renderer {
    public:
        SDL3Renderer();
        ~SDL3Renderer() override;

        bool Initialize(Window* window) override;
        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void SetCamera(const Camera& camera) override;
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override {}


        // Renderer2D
        void DrawQuad(float x, float y, float w, float h) override;
        void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle) override;
        Scope<Texture> CreateTexture(const std::string& filePath) override;
        Scope<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;

        // ImGui
        void SetImGuiDrawData(ImDrawData* drawData) override;

        SDL_Renderer* GetRenderer() const { return m_Renderer; }

        // No Implement
        void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) override { assert(false); }

    private:
        SDL_Renderer* m_Renderer = nullptr;
        float offsetX = 0.0f, offsetY = 0.0f;
        float zoom = 1.0f;
    };
}

#endif //AZER_SDL3RENDERER_H
