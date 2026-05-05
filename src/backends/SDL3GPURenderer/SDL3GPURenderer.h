//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_SDL3GPURENDERER_H
#define AZER_SDL3GPURENDERER_H

#include "Base.h"
#include "Renderer.h"
#include "glm/glm.hpp"

namespace azer
{
    class GPUTexture;

    class SDL3GPURenderer : public Renderer {
        friend class SDL3GPURendererSupport;

    public:
        SDL3GPURenderer();
        ~SDL3GPURenderer() override;

        bool Initialize(Window* window) override;
        void BeginFrame(const glm::vec3& clearColor) override;
        void EndFrame() override;
        void SetCamera(const Camera& camera) override;
        void SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY) override;

        // Renderer2D
        void DrawQuad(float x, float y, float w, float h) override;
        void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle) override;
        Scope<Texture> CreateTexture(const std::string& filePath) override;
        Scope<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;

        // Renderer3D
        void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) override;

        // ImGui
        void SetImGuiDrawData(ImDrawData* drawData) override;

        SDL_GPUDevice* GetDevice() const { return m_Device; }

        struct BatchVertex
        {
            float pos[2];
            float texCoord[2];
            float color[4];
        };

        struct BatchDrawCmd
        {
            SDL_GPUTexture* texture = nullptr;
            uint32_t vertexCount = 0;
            glm::mat4 mvp;
        };

    private:
        SDL_GPUDevice* m_Device = nullptr;
        SDL_Window* m_Window = nullptr;
        glm::mat4 m_MVPMatrix{};
        ImDrawData* m_ImGuiDrawData = nullptr;
        glm::vec3 m_ClearColor{};
        SDL_GPUViewport m_Viewport{};

        std::vector<BatchVertex> m_Vertices;
        std::vector<BatchDrawCmd> m_DrawCmds;

        SDL_GPUShader*           m_VertexShader = nullptr;
        SDL_GPUShader*           m_FragmentShader = nullptr;
        SDL_GPUGraphicsPipeline* m_Pipeline = nullptr;
        SDL_GPUBuffer*           m_VertexBuffer = nullptr;
        SDL_GPUSampler*          m_Sampler = nullptr;
        Scope<Texture>           m_WhiteTexture;
        uint32_t                 m_MaxVertices = 0;

        // For Vertices
        SDL_GPUTransferBuffer* m_VerticesTransferBuffer = nullptr;
    };
}

#endif //AZER_SDL3GPURENDERER_H
