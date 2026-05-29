//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_SDL3GPURENDERER_H
#define AZER_SDL3GPURENDERER_H

#include "Base.h"
#include "Renderer.h"
#include "glm/glm.hpp"
#include <unordered_map>

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
        void DrawQuad(float x, float y, float w, float h, float alpha) override;
        void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color, float alpha) override;
        void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle, float alpha) override;
        Ref<Texture> CreateTexture(const std::string& filePath) override;
        Ref<Texture> CreateTexture(void* pixels, uint32_t width, uint32_t height) override;
        Ref<Texture> CreateHDRTexture(const std::string& filePath) override;

        // Renderer3D
        void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) override;
        void DrawModel(Model& model, const glm::mat4& worldTransform, float alpha) override;
        void DrawSkybox(const Ref<Texture>& hdrTexture) override;

        // ImGui
        void ImGuiInit(SDL_Window* window) override;
        void ImGuiShutdown() override;
        void ImGuiNewFrame() override;
        void SetImGuiDrawData(ImDrawData* drawData) override;

        SDL_GPUDevice* GetDevice() const { return m_Device; }

        struct BatchVertex
        {
            float pos[3];
            float normal[3];
            float texCoord[2];
            float color[4];
        };

        struct UniformBufferObject
        {
            glm::mat4 viewProjection;
            glm::mat4 transform;
            float alpha = 1.0f;
            float _pad[3];
        };

        enum class PipelineType
        {
            Renderer2D,
            Renderer3D,
            Skybox
        };

        struct BatchDrawCmd
        {
            SDL_GPUTexture* texture = nullptr;
            SDL_GPUSampler* sampler = nullptr;
            uint32_t vertexCount = 0;
            UniformBufferObject ubo;
            PipelineType pipeline = PipelineType::Renderer2D;

            // Model rendering: overrides the shared vertex buffer
            SDL_GPUBuffer* vertexBufferOverride = nullptr;
            SDL_GPUBuffer* indexBuffer = nullptr;
        };
        // Model rendering
        struct GPUMeshData
        {
            SDL_GPUBuffer* VertexBuffer = nullptr;
            SDL_GPUBuffer* IndexBuffer = nullptr;
            uint32_t IndexCount = 0;
            bool UseIndexBuffer = false;
        };

    private:
        SDL_GPUDevice* m_Device = nullptr;
        SDL_Window* m_Window = nullptr;
        glm::mat4 m_MVPMatrix{};
        glm::mat4 m_SkyboxViewProjection{};
        ImDrawData* m_ImGuiDrawData = nullptr;
        glm::vec3 m_ClearColor{};
        SDL_GPUViewport m_Viewport{};

        std::vector<BatchVertex> m_Vertices;
        std::vector<BatchDrawCmd> m_DrawCmds;

        SDL_GPUShader*           m_VertexShader = nullptr;
        SDL_GPUShader*           m_FragmentShader = nullptr;
        SDL_GPUShader*           m_SkyboxVertexShader = nullptr;
        SDL_GPUShader*           m_SkyboxFragmentShader = nullptr;

        SDL_GPUGraphicsPipeline* m_Pipeline2D = nullptr;
        SDL_GPUGraphicsPipeline* m_Pipeline3D = nullptr;
        SDL_GPUGraphicsPipeline* m_PipelineSkybox = nullptr;
        SDL_GPUTexture*          m_DepthTexture = nullptr;
        SDL_GPUTextureFormat     m_DepthTextureFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
        uint32_t                 m_DepthTextureWidth = 0;
        uint32_t                 m_DepthTextureHeight = 0;

        SDL_GPUBuffer*           m_VertexBuffer = nullptr;
        SDL_GPUSampler*          m_Sampler = nullptr;
        SDL_GPUSampler*          m_SkyboxSampler = nullptr;
        Ref<Texture>           m_WhiteTexture;
        uint32_t                 m_MaxVertices = 0;

        // For Vertices
        SDL_GPUTransferBuffer* m_VerticesTransferBuffer = nullptr;

        std::unordered_map<Model*, std::vector<GPUMeshData>> m_ModelCache;

        bool EnsureDepthTexture(uint32_t width, uint32_t height);
        void ReleaseDepthTexture();
    };
}

#endif //AZER_SDL3GPURENDERER_H
