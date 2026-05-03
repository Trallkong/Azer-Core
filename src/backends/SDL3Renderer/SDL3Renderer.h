//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_SDL3RENDERER_H
#define AZER_SDL3RENDERER_H
#include "Renderer.h"

namespace azer
{
    class SDL3Renderer : public Renderer {
    public:
        SDL3Renderer();
        ~SDL3Renderer() override;

        bool Initialize(SDL_Window* window) override;
        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void SetCamera(const CameraConfig& config) override;

        // Renderer2D
        void DrawQuad(float x, float y, float w, float h) override;
        void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle) override;
        Scope<Texture> CreateTexture(const std::string& filePath) override;

        // ImGui
        void SetImGuiDrawData(ImDrawData* drawData) override;

        SDL_Renderer* GetRenderer() const { return m_Renderer; }
    private:
        SDL_Renderer* m_Renderer = nullptr;
        float offsetX = 0.0f, offsetY = 0.0f;
        float zoom = 1.0f;
    };
}

#endif //AZER_SDL3RENDERER_H
