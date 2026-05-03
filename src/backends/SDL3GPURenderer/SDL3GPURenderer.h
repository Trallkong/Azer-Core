//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_SDL3GPURENDERER_H
#define AZER_SDL3GPURENDERER_H
#include "Renderer.h"
#include "glm/glm.hpp"

namespace azer
{
    class SDL3GPURenderer : public Renderer {
    public:
        SDL3GPURenderer();
        ~SDL3GPURenderer() override;

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

        SDL_GPUDevice* GetDevice() const { return m_Device; }


    private:
        SDL_GPUDevice* m_Device = nullptr;
        SDL_Window* m_Window = nullptr;
        glm::mat4 m_MVPMatrix;
        ImDrawData* m_ImGuiDrawData = nullptr;
        glm::vec3 m_ClearColor;
    };
}

#endif //AZER_SDL3GPURENDERER_H
