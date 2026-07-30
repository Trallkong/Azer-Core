//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3GPURenderer.h"
#include "SDL3GPURendererSupport.h"

#include "GPUTexture.h"
#include "SDL3GPUFramebuffer.h"
#include "Mesh2D.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"


namespace Azer
{
    static void DestroyGPUMeshData(SDL_GPUDevice* device, const SDL3GPURenderer::GPUMeshData& data)
    {
        if (data.VertexBuffer) SDL_ReleaseGPUBuffer(device, data.VertexBuffer);
        if (data.IndexBuffer) SDL_ReleaseGPUBuffer(device, data.IndexBuffer);
    }

    bool SDL3GPURenderer::Initialize(Window* window)
    {
        m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
        if (!m_Device)
        {
            AZ_CORE_ERROR("Failed to create gpu device: {0}", SDL_GetError());
            return false;
        }

        auto* sdl_window = static_cast<SDL_Window*>(window->GetHandle());

        if (!SDL_ClaimWindowForGPUDevice(m_Device, sdl_window))
        {
            AZ_CORE_ERROR("Failed to claim window for gpu device: {0}", SDL_GetError());
            return false;
        }
        m_Window = sdl_window;
        m_MaxVertices = 10000;

        SDL3GPURendererSupport::CreateVerticesTransferBuffer(this);
        SDL3GPURendererSupport::CreateSampler(this);
        SDL3GPURendererSupport::CreateSkyboxSampler(this);
        SDL3GPURendererSupport::CreateWhiteTexture(this);
        SDL3GPURendererSupport::CreateShaders(this);
        SDL3GPURendererSupport::CreateGraphicsPipeline2D(this);
        SDL3GPURendererSupport::CreateGraphicsPipeline3D(this);
        SDL3GPURendererSupport::CreateGraphicsPipelineSkybox(this);
        SDL3GPURendererSupport::CreateBuffers(this);

        int winW = 1280, winH = 720;
        SDL_GetWindowSize(m_Window, &winW, &winH);

        m_Viewport.w = static_cast<float>(winW);
        m_Viewport.h = static_cast<float>(winH);
        m_Viewport.x = 0;
        m_Viewport.y = 0;
        m_Viewport.min_depth = 0;
        m_Viewport.max_depth = 1;

        return true;
    }

    void SDL3GPURenderer::Shutdown()
    {
        m_Vertices.clear();
        m_DrawCmds.clear();

        for (auto& [model, meshes] : m_ModelCache)
            for (auto& mesh : meshes)
                DestroyGPUMeshData(m_Device, mesh);
        m_ModelCache.clear();

        m_WhiteTexture.reset();

        ImGuiShutdown();

        if (m_VerticesTransferBuffer) SDL_ReleaseGPUTransferBuffer(m_Device, m_VerticesTransferBuffer);
        if (m_Sampler)         SDL_ReleaseGPUSampler(m_Device, m_Sampler);
        if (m_SkyboxSampler)   SDL_ReleaseGPUSampler(m_Device, m_SkyboxSampler);
        if (m_VertexBuffer)    SDL_ReleaseGPUBuffer(m_Device, m_VertexBuffer);
        ReleaseDepthTexture();
        if (m_Pipeline2D)        SDL_ReleaseGPUGraphicsPipeline(m_Device, m_Pipeline2D);
        if (m_Pipeline3D)        SDL_ReleaseGPUGraphicsPipeline(m_Device, m_Pipeline3D);
        if (m_PipelineSkybox)    SDL_ReleaseGPUGraphicsPipeline(m_Device, m_PipelineSkybox);
        if (m_FragmentShader)  SDL_ReleaseGPUShader(m_Device, m_FragmentShader);
        if (m_VertexShader)    SDL_ReleaseGPUShader(m_Device, m_VertexShader);
        if (m_SkyboxFragmentShader) SDL_ReleaseGPUShader(m_Device, m_SkyboxFragmentShader);
        if (m_SkyboxVertexShader)   SDL_ReleaseGPUShader(m_Device, m_SkyboxVertexShader);
        if (m_Window)          SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
        if (m_Device)          SDL_DestroyGPUDevice(m_Device);
    }

    void SDL3GPURenderer::BeginFrame(const glm::vec3& clearColor)
    {
        m_ClearColor = clearColor;
        m_Vertices.clear();
        m_DrawCmds.clear();
    }

    void SDL3GPURenderer::EndFrame()
    {
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_Device);
        if (!cmd) return;

        // Prepare draw data
        if (!SDL3GPURendererSupport::PrepareDrawData(this, cmd)) return;

        // ImGui prepare
        if (m_ImGuiDrawData)
            ImGui_ImplSDLGPU3_PrepareDrawData(m_ImGuiDrawData, cmd);

        // Acquire swapchain
        SDL_GPUTexture* swapchain = nullptr;
        Uint32 scW, scH;
        SDL_AcquireGPUSwapchainTexture(cmd, m_Window, &swapchain, &scW, &scH);
        if (!swapchain) {
            m_Vertices.clear();
            m_DrawCmds.clear();
            m_ImGuiDrawData = nullptr;
            SDL_SubmitGPUCommandBuffer(cmd);
            return;
        }
        if (!EnsureDepthTexture(scW, scH))
        {
            m_Vertices.clear();
            m_DrawCmds.clear();
            m_ImGuiDrawData = nullptr;
            SDL_SubmitGPUCommandBuffer(cmd);
            return;
        }

        // 按 render target 分组 draw cmds
        std::unordered_map<Framebuffer*, std::vector<BatchDrawCmd>> grouped;
        for (const auto& dc : m_DrawCmds)
            grouped[dc.target].push_back(dc);

        // 预计算每组的 baseVertex 偏移（顶点缓冲按 cmd 顺序排列）
        std::unordered_map<Framebuffer*, Uint32> groupBaseVertex;
        {
            Uint32 offset = 0;
            for (const auto& dc : m_DrawCmds)
            {
                if (!groupBaseVertex.count(dc.target))
                    groupBaseVertex[dc.target] = offset;
                if (!dc.vertexBufferOverride)
                    offset += dc.vertexCount;
            }
        }

        // 渲染所有 framebuffer 目标
        for (auto& [target, cmds] : grouped)
        {
            if (!target) continue;  // swapchain 在后面单独处理

            auto* colorTex = static_cast<SDL_GPUTexture*>(target->GetColorTextureHandle());
            auto* depthTex = static_cast<SDL_GPUTexture*>(target->GetDepthTextureHandle());
            Uint32 bv = groupBaseVertex[target];

            RenderBatch(cmd, colorTex, depthTex, target->GetWidth(), target->GetHeight(), cmds, bv, true);
        }

        // 渲染 swapchain 目标（包括 default target 的 draw cmds）
        auto it = grouped.find(nullptr);
        if (it != grouped.end() && !it->second.empty())
        {
            Uint32 bv = groupBaseVertex[nullptr];
            RenderBatch(cmd, swapchain, m_DepthTexture, scW, scH, it->second, bv, true);
        }
        else
        {
            // 没有 swapchain 的 draw cmds，但需要清屏
            SDL_GPUColorTargetInfo ct{};
            ct.texture = swapchain;
            ct.clear_color = {m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f};
            ct.load_op = SDL_GPU_LOADOP_CLEAR;
            ct.store_op = SDL_GPU_STOREOP_STORE;
            SDL_GPURenderPass* rp = SDL_BeginGPURenderPass(cmd, &ct, 1, nullptr);
            SDL_EndGPURenderPass(rp);
        }

        // ImGui pass (always on swapchain)
        if (m_ImGuiDrawData)
        {
            SDL_GPUColorTargetInfo imguiCt{};
            imguiCt.texture = swapchain;
            imguiCt.load_op = SDL_GPU_LOADOP_LOAD;
            imguiCt.store_op = SDL_GPU_STOREOP_STORE;
            SDL_GPURenderPass* rp = SDL_BeginGPURenderPass(cmd, &imguiCt, 1, nullptr);
            ImGui_ImplSDLGPU3_RenderDrawData(m_ImGuiDrawData, cmd, rp, nullptr);
            SDL_EndGPURenderPass(rp);
        }

        SDL_SubmitGPUCommandBuffer(cmd);

        m_Vertices.clear();
        m_DrawCmds.clear();
        m_ImGuiDrawData = nullptr;
    }

    void SDL3GPURenderer::RenderBatch(SDL_GPUCommandBuffer* cmd, SDL_GPUTexture* colorTex, void* depthTex,
        uint32_t w, uint32_t h, const std::vector<BatchDrawCmd>& cmds, Uint32& baseVertex, bool clear)
    {
        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = colorTex;
        colorTarget.clear_color = {m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f};
        colorTarget.load_op = clear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthTarget{};
        if (depthTex)
        {
            depthTarget.texture = static_cast<SDL_GPUTexture*>(depthTex);
            depthTarget.clear_depth = 1.0f;
            depthTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            depthTarget.store_op = SDL_GPU_STOREOP_DONT_CARE;
            depthTarget.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
            depthTarget.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
        }

        SDL_GPURenderPass* rp = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, depthTex ? &depthTarget : nullptr);

        SDL_GPUViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.w = static_cast<float>(w);
        viewport.h = static_cast<float>(h);
        viewport.min_depth = 0.0f;
        viewport.max_depth = 1.0f;
        SDL_SetGPUViewport(rp, &viewport);

        if (!cmds.empty())
        {
            SDL_GPUGraphicsPipeline* currentPipeline = nullptr;
            for (const auto& drawCmd : cmds)
            {
                SDL_GPUGraphicsPipeline* pipeline = m_Pipeline2D;
                if (drawCmd.pipeline == PipelineType::Renderer3D)
                    pipeline = m_Pipeline3D;
                else if (drawCmd.pipeline == PipelineType::Skybox)
                    pipeline = m_PipelineSkybox;
                if (currentPipeline != pipeline)
                {
                    SDL_BindGPUGraphicsPipeline(rp, pipeline);
                    currentPipeline = pipeline;
                }

                SDL_PushGPUVertexUniformData(cmd, 0, &drawCmd.ubo, sizeof(UniformBufferObject));

                SDL_GPUTextureSamplerBinding texBinding{};
                texBinding.texture = drawCmd.texture;
                texBinding.sampler = drawCmd.sampler ? drawCmd.sampler : m_Sampler;
                SDL_BindGPUFragmentSamplers(rp, 0, &texBinding, 1);

                SDL_GPUBufferBinding vtxBinding{};
                vtxBinding.buffer = drawCmd.vertexBufferOverride ? drawCmd.vertexBufferOverride : m_VertexBuffer;
                vtxBinding.offset = 0;
                SDL_BindGPUVertexBuffers(rp, 0, &vtxBinding, 1);

                if (drawCmd.indexBuffer)
                {
                    SDL_GPUBufferBinding idxBinding{};
                    idxBinding.buffer = drawCmd.indexBuffer;
                    idxBinding.offset = 0;
                    SDL_BindGPUIndexBuffer(rp, &idxBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
                    SDL_DrawGPUIndexedPrimitives(rp, drawCmd.vertexCount, 1, 0, 0, 0);
                }
                else
                {
                    SDL_DrawGPUPrimitives(rp, drawCmd.vertexCount, 1, baseVertex, 0);
                    baseVertex += drawCmd.vertexCount;
                }
            }
        }

        SDL_EndGPURenderPass(rp);
    }

    void SDL3GPURenderer::SetCamera(Camera& camera)
    {
        m_ViewProjectionMatrix = camera.GetViewProjectionMatrix();
        m_SkyboxViewProjection = camera.GetProjectionMatrix() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
    }

    void SDL3GPURenderer::ResetRenderState()
    {
        m_ViewProjectionMatrix = glm::mat4(1.0f);
        m_CurrentTarget = nullptr;
    }

    void SDL3GPURenderer::SetRenderTarget(Framebuffer* target)
    {
        m_CurrentTarget = target;
    }

    void SDL3GPURenderer::SetViewport(const uint32_t width, const uint32_t height, const uint32_t offsetX, const uint32_t offsetY)
    {
        m_Viewport.w = static_cast<float>(width);
        m_Viewport.h = static_cast<float>(height);
        m_Viewport.x = static_cast<float>(offsetX);
        m_Viewport.y = static_cast<float>(offsetY);
        ReleaseDepthTexture();
    }

    bool SDL3GPURenderer::EnsureDepthTexture(uint32_t width, uint32_t height)
    {
        if (m_DepthTexture && m_DepthTextureWidth == width && m_DepthTextureHeight == height)
            return true;

        ReleaseDepthTexture();

        SDL_GPUTextureCreateInfo info {};
        info.type = SDL_GPU_TEXTURETYPE_2D;
        info.format = m_DepthTextureFormat;
        info.width = width;
        info.height = height;
        info.layer_count_or_depth = 1;
        info.num_levels = 1;
        info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;

        m_DepthTexture = SDL_CreateGPUTexture(m_Device, &info);
        if (!m_DepthTexture)
        {
            AZ_CORE_ERROR("Failed to create depth texture: {0}", SDL_GetError());
            return false;
        }

        m_DepthTextureWidth = width;
        m_DepthTextureHeight = height;
        return true;
    }

    void SDL3GPURenderer::ReleaseDepthTexture()
    {
        if (m_DepthTexture)
        {
            SDL_ReleaseGPUTexture(m_Device, m_DepthTexture);
            m_DepthTexture = nullptr;
        }
        m_DepthTextureWidth = 0;
        m_DepthTextureHeight = 0;
    }

    void SDL3GPURenderer::DrawQuad(float x, float y, float w, float h, float alpha)
    {
        DrawColorQuad(x, y, w, h, {1.0f, 1.0f, 1.0f, 1.0f}, alpha);
    }

    void SDL3GPURenderer::DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color, float alpha)
    {
        const float n[3] = {0, 0, 1};

        QuadMesh quadMesh;
        quadMesh.SetSize({w, h});
        quadMesh.SetColor(color);
        const auto& verts = quadMesh.GetVertices();
        const auto& idxs = quadMesh.GetIndices();

        for (auto i : idxs)
        {
            const auto& v = verts[i];
            m_Vertices.push_back({
                {v.position.x, v.position.y, v.position.z},
                {n[0], n[1], n[2]},
                {v.uv.x, v.uv.y},
                {v.color.r, v.color.g, v.color.b, v.color.a}
            });
        }

        BatchDrawCmd cmd {};
        cmd.target = m_CurrentTarget;
        cmd.texture = static_cast<SDL_GPUTexture*>(m_WhiteTexture->GetHandle());
        cmd.vertexCount = static_cast<uint32_t>(idxs.size());
        cmd.pipeline = PipelineType::Renderer2D;

        UniformBufferObject ubo {};
        ubo.viewProjection = m_ViewProjectionMatrix;
        ubo.transform = glm::mat4(1.0f);
        ubo.alpha = alpha;

        cmd.ubo = ubo;
        m_DrawCmds.push_back(cmd);
    }

    void SDL3GPURenderer::DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle, float alpha)
    {
        const auto* gpuTex = dynamic_cast<GPUTexture*>(tex);
        const auto handle = static_cast<SDL_GPUTexture*>(gpuTex->GetHandle());
        const auto tw = static_cast<float>(gpuTex->GetWidth());
        const auto th = static_cast<float>(gpuTex->GetHeight());

        const float u0 = src.x / tw;
        const float v0 = src.y / th;
        const float u1 = (src.x + src.w) / tw;
        const float v1 = (src.y + src.h) / th;

        const float x0 = dst.x;
        const float y0 = dst.y;
        const float x1 = dst.x + dst.w;
        const float y1 = dst.y + dst.h;

        const float n[3] = {0, 0, 1};

        // Triangle 1
        m_Vertices.push_back({{x1, y0, 0.0f}, {n[0],n[1],n[2]}, {u1, v0}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y0, 0.0f}, {n[0],n[1],n[2]}, {u0, v0}, {1,1,1,1}});
        m_Vertices.push_back({{x1, y1, 0.0f}, {n[0],n[1],n[2]}, {u1, v1}, {1,1,1,1}});
        // Triangle 2
        m_Vertices.push_back({{x1, y1, 0.0f}, {n[0],n[1],n[2]}, {u1, v1}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y1, 0.0f}, {n[0],n[1],n[2]}, {u0, v1}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y0, 0.0f}, {n[0],n[1],n[2]}, {u0, v0}, {1,1,1,1}});

        BatchDrawCmd cmd {};
        cmd.target = m_CurrentTarget;        cmd.texture = handle;
        cmd.vertexCount = 6;
        cmd.pipeline = PipelineType::Renderer2D;

        UniformBufferObject ubo {};
        ubo.viewProjection = m_ViewProjectionMatrix;
        ubo.transform = glm::mat4(1.0f);
        ubo.alpha = alpha;

        cmd.ubo = ubo;
        m_DrawCmds.push_back(cmd);
    }

    Ref<Texture> SDL3GPURenderer::CreateTexture(const std::string& filePath)
    {
        return GPUTexture::Create(m_Device, filePath);
    }

    Ref<Texture> SDL3GPURenderer::CreateTexture(void* pixels, uint32_t width, uint32_t height)
    {
        return GPUTexture::Create(m_Device, pixels, width, height);
    }

    Ref<Texture> SDL3GPURenderer::CreateHDRTexture(const std::string& filePath)
    {
        return GPUTexture::CreateHDR(m_Device, filePath);
    }

    Ref<Framebuffer> SDL3GPURenderer::CreateFramebuffer(const FramebufferSpec& spec)
    {
        return CreateRef<SDL3GPUFramebuffer>(m_Device, spec);
    }

    void SDL3GPURenderer::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        // CCW winding from outside for back-face culling
        const glm::vec3 cubeFaces[6][4] = {
            {{ 1,-1, 1}, { 1,-1,-1}, { 1, 1,-1}, { 1, 1, 1}}, // +X
            {{-1,-1,-1}, {-1,-1, 1}, {-1, 1, 1}, {-1, 1,-1}}, // -X
            {{-1, 1, 1}, { 1, 1, 1}, { 1, 1,-1}, {-1, 1,-1}}, // +Y
            {{-1,-1,-1}, { 1,-1,-1}, { 1,-1, 1}, {-1,-1, 1}}, // -Y
            {{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}}, // +Z
            {{ 1,-1,-1}, {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}}, // -Z
        };

        const float faceColors[6][4] = {
            {1.0f, 0.0f, 0.0f, 1.0f}, // +X red
            {0.0f, 1.0f, 0.0f, 1.0f}, // -X green
            {0.0f, 0.0f, 1.0f, 1.0f}, // +Y blue
            {1.0f, 1.0f, 0.0f, 1.0f}, // -Y yellow
            {1.0f, 0.0f, 1.0f, 1.0f}, // +Z magenta
            {0.0f, 1.0f, 1.0f, 1.0f}, // -Z cyan
        };

        const float faceNormals[6][3] = {
            { 1, 0, 0}, // +X
            {-1, 0, 0}, // -X
            { 0, 1, 0}, // +Y
            { 0,-1, 0}, // -Y
            { 0, 0, 1}, // +Z
            { 0, 0,-1}, // -Z
        };

        auto* whiteTex = static_cast<SDL_GPUTexture*>(m_WhiteTexture->GetHandle());

        UniformBufferObject ubo {};
        ubo.viewProjection = m_ViewProjectionMatrix;
        ubo.transform = model;

        for (int f = 0; f < 6; ++f)
        {
            const float* c = faceColors[f];
            const float* n = faceNormals[f];

            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][1].x, cubeFaces[f][1].y, cubeFaces[f][1].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][3].x, cubeFaces[f][3].y, cubeFaces[f][3].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});

            BatchDrawCmd cmd {};
        cmd.target = m_CurrentTarget;            cmd.texture = whiteTex;
            cmd.vertexCount = 6;
            cmd.pipeline = PipelineType::Renderer3D;
            cmd.ubo = ubo;
            m_DrawCmds.push_back(cmd);
        }
    }

    void SDL3GPURenderer::DrawSkybox(const Ref<Texture>& hdrTexture)
    {
        if (!hdrTexture)
            return;

        auto* texture = static_cast<SDL_GPUTexture*>(hdrTexture->GetHandle());
        const float c[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        const float n[3] = {0.0f, 0.0f, 0.0f};

        const glm::vec3 cubeFaces[6][4] = {
            {{ 1,-1, 1}, { 1,-1,-1}, { 1, 1,-1}, { 1, 1, 1}}, // +X
            {{-1,-1,-1}, {-1,-1, 1}, {-1, 1, 1}, {-1, 1,-1}}, // -X
            {{-1, 1, 1}, { 1, 1, 1}, { 1, 1,-1}, {-1, 1,-1}}, // +Y
            {{-1,-1,-1}, { 1,-1,-1}, { 1,-1, 1}, {-1,-1, 1}}, // -Y
            {{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1}}, // +Z
            {{ 1,-1,-1}, {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}}, // -Z
        };

        for (int f = 0; f < 6; ++f)
        {
            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][1].x, cubeFaces[f][1].y, cubeFaces[f][1].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][3].x, cubeFaces[f][3].y, cubeFaces[f][3].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {n[0],n[1],n[2]}, {0, 0}, {c[0], c[1], c[2], c[3]}});

            BatchDrawCmd cmd {};
        cmd.target = m_CurrentTarget;            cmd.texture = texture;
            cmd.sampler = m_SkyboxSampler;
            cmd.vertexCount = 6;
            cmd.pipeline = PipelineType::Skybox;

            UniformBufferObject ubo {};
            ubo.viewProjection = m_SkyboxViewProjection;
            ubo.transform = glm::mat4(1.0f);
            cmd.ubo = ubo;

            m_DrawCmds.push_back(cmd);
        }
    }

    static SDL3GPURenderer::GPUMeshData CreateGPUMesh(SDL_GPUDevice* device,
                                                       const std::vector<SDL3GPURenderer::BatchVertex>& vertices,
                                                       const std::vector<uint32_t>& indices)
    {
        SDL3GPURenderer::GPUMeshData data{};

        SDL_GPUBufferCreateInfo vtxInfo {};
        vtxInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vtxInfo.size  = static_cast<Uint32>(vertices.size() * sizeof(SDL3GPURenderer::BatchVertex));
        data.VertexBuffer = SDL_CreateGPUBuffer(device, &vtxInfo);

        SDL_GPUBufferCreateInfo idxInfo {};
        if (!indices.empty())
        {
            idxInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
            idxInfo.size  = static_cast<Uint32>(indices.size() * sizeof(uint32_t));
            data.IndexBuffer = SDL_CreateGPUBuffer(device, &idxInfo);
            data.IndexCount = static_cast<uint32_t>(indices.size());
            data.UseIndexBuffer = true;
        }

        SDL_GPUTransferBufferCreateInfo uploadInfo {};
        uploadInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        uploadInfo.size = vtxInfo.size;
        if (data.IndexBuffer)
            uploadInfo.size += idxInfo.size;

        SDL_GPUTransferBuffer* uploadBuf = SDL_CreateGPUTransferBuffer(device, &uploadInfo);
        if (!uploadBuf) return data;

        void* mapped = SDL_MapGPUTransferBuffer(device, uploadBuf, false);
        if (!mapped) { SDL_ReleaseGPUTransferBuffer(device, uploadBuf); return data; }

        Uint8* ptr = static_cast<Uint8*>(mapped);
        memcpy(ptr, vertices.data(), vtxInfo.size);
        ptr += vtxInfo.size;
        if (data.IndexBuffer)
            memcpy(ptr, indices.data(), idxInfo.size);

        SDL_UnmapGPUTransferBuffer(device, uploadBuf);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
        if (!cmd) { SDL_ReleaseGPUTransferBuffer(device, uploadBuf); return data; }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTransferBufferLocation srcVtx {};
        srcVtx.transfer_buffer = uploadBuf;
        srcVtx.offset = 0;
        SDL_GPUBufferRegion dstVtx {};
        dstVtx.buffer = data.VertexBuffer;
        dstVtx.offset = 0;
        dstVtx.size = vtxInfo.size;
        SDL_UploadToGPUBuffer(copyPass, &srcVtx, &dstVtx, false);

        if (data.IndexBuffer)
        {
            SDL_GPUTransferBufferLocation srcIdx {};
            srcIdx.transfer_buffer = uploadBuf;
            srcIdx.offset = vtxInfo.size;
            SDL_GPUBufferRegion dstIdx {};
            dstIdx.buffer = data.IndexBuffer;
            dstIdx.offset = 0;
            dstIdx.size = idxInfo.size;
            SDL_UploadToGPUBuffer(copyPass, &srcIdx, &dstIdx, false);
        }

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(device, uploadBuf);

        return data;
    }

    void SDL3GPURenderer::DrawModel(Model& model, const glm::mat4& worldTransform, float alpha)
    {
        auto it = m_ModelCache.find(&model);
        if (it == m_ModelCache.end())
        {
            std::vector<GPUMeshData> meshes;
            for (const auto& mesh : model.GetMeshes())
            {
                std::vector<BatchVertex> batchVerts;
                batchVerts.reserve(mesh.Vertices.size());
                for (const auto& v : mesh.Vertices)
                {
                    BatchVertex bv{};
                    bv.pos[0] = v.Position.x; bv.pos[1] = v.Position.y; bv.pos[2] = v.Position.z;
                    bv.normal[0] = v.Normal.x; bv.normal[1] = v.Normal.y; bv.normal[2] = v.Normal.z;
                    bv.texCoord[0] = v.TexCoord.x; bv.texCoord[1] = v.TexCoord.y;
                    bv.color[0] = 1.0f; bv.color[1] = 1.0f; bv.color[2] = 1.0f; bv.color[3] = 1.0f;
                    batchVerts.push_back(bv);
                }
                meshes.push_back(CreateGPUMesh(m_Device, batchVerts, mesh.Indices));
            }
            it = m_ModelCache.emplace(&model, std::move(meshes)).first;
        }

        const auto& gpuMeshes = it->second;

        const auto& materials = model.GetMaterials();
        const auto& textures = model.GetTextures();

        model.Traverse([&](const ModelNode& node, uint32_t meshIdx, const Mesh& mesh, const glm::mat4& nodeTransform)
        {
            if (meshIdx >= gpuMeshes.size()) return;
            const auto& gpuData = gpuMeshes[meshIdx];

            SDL_GPUTexture* texHandle = static_cast<SDL_GPUTexture*>(m_WhiteTexture->GetHandle());
            if (mesh.MaterialIndex < materials.size())
            {
                const auto& mat = materials[mesh.MaterialIndex];
                int32_t texIdx = mat.BaseColorTexIndex;
                if (texIdx >= 0 && static_cast<size_t>(texIdx) < textures.size() && textures[texIdx])
                    texHandle = static_cast<SDL_GPUTexture*>(textures[texIdx]->GetHandle());
            }

            BatchDrawCmd cmd {};
        cmd.target = m_CurrentTarget;            cmd.texture = texHandle;
            cmd.vertexCount = gpuData.IndexCount > 0 ? gpuData.IndexCount : static_cast<uint32_t>(mesh.Vertices.size());
            cmd.pipeline = PipelineType::Renderer3D;
            cmd.vertexBufferOverride = gpuData.VertexBuffer;
            cmd.indexBuffer = gpuData.UseIndexBuffer ? gpuData.IndexBuffer : nullptr;

            UniformBufferObject ubo {};
            ubo.viewProjection = m_ViewProjectionMatrix;
            ubo.transform = worldTransform * nodeTransform;
            ubo.alpha = alpha;
            cmd.ubo = ubo;

            m_DrawCmds.push_back(cmd);
        });
    }

    void SDL3GPURenderer::ImGuiInit(SDL_Window* window)
    {
        ImGui_ImplSDL3_InitForSDLGPU(window);
        ImGui_ImplSDLGPU3_InitInfo init_info = {};
        init_info.Device = m_Device;
        init_info.ColorTargetFormat = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
        ImGui_ImplSDLGPU3_Init(&init_info);
    }

    void SDL3GPURenderer::ImGuiShutdown()
    {
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }

    void SDL3GPURenderer::ImGuiNewFrame()
    {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
    }

    void SDL3GPURenderer::SetImGuiDrawData(ImDrawData* drawData)
    {
        m_ImGuiDrawData = drawData;
    }
}
