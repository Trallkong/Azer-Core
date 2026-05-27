//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "SDL3GPURenderer.h"
#include "SDL3GPURendererSupport.h"

#include "GPUTexture.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"


namespace azer
{
    SDL3GPURenderer::SDL3GPURenderer()
    = default;

    SDL3GPURenderer::~SDL3GPURenderer()
    {
        m_Vertices.clear();
        m_DrawCmds.clear();

        m_WhiteTexture.reset();

        if (m_VerticesTransferBuffer) SDL_ReleaseGPUTransferBuffer(m_Device, m_VerticesTransferBuffer);
        if (m_Sampler)         SDL_ReleaseGPUSampler(m_Device, m_Sampler);
        if (m_VertexBuffer)    SDL_ReleaseGPUBuffer(m_Device, m_VertexBuffer);
        if (m_Pipeline2D)        SDL_ReleaseGPUGraphicsPipeline(m_Device, m_Pipeline2D);
        if (m_Pipeline3D)        SDL_ReleaseGPUGraphicsPipeline(m_Device, m_Pipeline3D);
        if (m_FragmentShader)  SDL_ReleaseGPUShader(m_Device, m_FragmentShader);
        if (m_VertexShader)    SDL_ReleaseGPUShader(m_Device, m_VertexShader);
        if (m_Window)          SDL_ReleaseWindowFromGPUDevice(m_Device, m_Window);
        if (m_Device)          SDL_DestroyGPUDevice(m_Device);
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
        SDL3GPURendererSupport::CreateWhiteTexture(this);
        SDL3GPURendererSupport::CreateShaders(this);
        SDL3GPURendererSupport::CreateGraphicsPipeline2D(this);
        SDL3GPURendererSupport::CreateGraphicsPipeline3D(this);
        SDL3GPURendererSupport::CreateBuffers(this);

        int winW = 1280, winH = 720;
        SDL_GetWindowSize(m_Window, &winW, &winH);
        m_MVPMatrix = glm::ortho(0.0f, static_cast<float>(winW),
                                 static_cast<float>(winH), 0.0f,
                                 -1.0f, 1.0f);

        m_Viewport.w = 1280;
        m_Viewport.h = 720;
        m_Viewport.x = 0;
        m_Viewport.y = 0;
        m_Viewport.min_depth = 0;
        m_Viewport.max_depth = 1;

        return true;
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

        // My prepare include camera set
        if (!SDL3GPURendererSupport::PrepareDrawData(this, cmd)) return;

        // ImGui prepare
        if (m_ImGuiDrawData)
            ImGui_ImplSDLGPU3_PrepareDrawData(m_ImGuiDrawData, cmd);

        // === Render Pass ===
        SDL_GPUTexture* swapchain = nullptr;
        Uint32 w, h;
        SDL_AcquireGPUSwapchainTexture(cmd, m_Window, &swapchain, &w, &h);
        if (!swapchain) {
            m_Vertices.clear();
            m_DrawCmds.clear();
            m_ImGuiDrawData = nullptr;
            SDL_SubmitGPUCommandBuffer(cmd);
            return;
        }

        SDL_GPUColorTargetInfo colorTarget {};
        colorTarget.texture = swapchain;
        colorTarget.clear_color = {m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f};
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* rp = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, nullptr);

        SDL_SetGPUViewport(rp, &m_Viewport);

        if (!m_Vertices.empty())
        {
            SDL_GPUBufferBinding vtxBinding {};
            vtxBinding.buffer = m_VertexBuffer;
            vtxBinding.offset = 0;
            SDL_BindGPUVertexBuffers(rp, 0, &vtxBinding, 1);

            Uint32 baseVertex = 0;
            SDL_GPUGraphicsPipeline* currentPipeline = nullptr;
            for (const auto& drawCmd : m_DrawCmds)
            {
                auto* pipeline = drawCmd.is3D ? m_Pipeline3D : m_Pipeline2D;
                if (currentPipeline != pipeline)
                {
                    SDL_BindGPUGraphicsPipeline(rp, pipeline);
                    currentPipeline = pipeline;
                }

                SDL_PushGPUVertexUniformData(cmd, 0, &drawCmd.ubo, sizeof(UniformBufferObject));

                SDL_GPUTextureSamplerBinding texBinding {};
                texBinding.texture = drawCmd.texture;
                texBinding.sampler = m_Sampler;
                SDL_BindGPUFragmentSamplers(rp, 0, &texBinding, 1);

                SDL_DrawGPUPrimitives(rp, drawCmd.vertexCount, 1, baseVertex, 0);
                baseVertex += drawCmd.vertexCount;
            }
        }

        if (m_ImGuiDrawData)
            ImGui_ImplSDLGPU3_RenderDrawData(m_ImGuiDrawData, cmd, rp, nullptr);

        SDL_EndGPURenderPass(rp);
        SDL_SubmitGPUCommandBuffer(cmd);

        m_Vertices.clear();
        m_DrawCmds.clear();
        m_ImGuiDrawData = nullptr;
    }

    void SDL3GPURenderer::SetCamera(const Camera& camera)
    {
        m_MVPMatrix = camera.GetViewProjection(m_Viewport.w, m_Viewport.h);
    }

    void SDL3GPURenderer::SetViewport(const uint32_t width, const uint32_t height, const uint32_t offsetX, const uint32_t offsetY)
    {
        m_Viewport.w = static_cast<float>(width);
        m_Viewport.h = static_cast<float>(height);
        m_Viewport.x = static_cast<float>(offsetX);
        m_Viewport.y = static_cast<float>(offsetY);
    }

    void SDL3GPURenderer::DrawQuad(float x, float y, float w, float h)
    {
        DrawColorQuad(x, y, w, h, {1.0f, 1.0f, 1.0f, 1.0f});
    }

    void SDL3GPURenderer::DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color)
    {
        const float x2 = x + w, y2 = y + h;
        const float c[4] = {color.r, color.g, color.b, color.a};

        // Triangle 1: v0, v1, v2
        m_Vertices.push_back({{x,  y, 0.0f }, {0,0}, {c[0],c[1],c[2],c[3]}});
        m_Vertices.push_back({{x2, y, 0.0f}, {1,0}, {c[0],c[1],c[2],c[3]}});
        m_Vertices.push_back({{x2, y2, 0.0f}, {1,1}, {c[0],c[1],c[2],c[3]}});
        // Triangle 2: v2, v3, v0
        m_Vertices.push_back({{x2, y2, 0.0f}, {1,1}, {c[0],c[1],c[2],c[3]}});
        m_Vertices.push_back({{x,  y2, 0.0f}, {0,1}, {c[0],c[1],c[2],c[3]}});
        m_Vertices.push_back({{x,  y, 0.0f }, {0,0}, {c[0],c[1],c[2],c[3]}});

        BatchDrawCmd cmd {};
        cmd.texture = static_cast<SDL_GPUTexture*>(m_WhiteTexture->GetHandle());
        cmd.vertexCount = 6;
        cmd.is3D = false;

        UniformBufferObject ubo {};
        ubo.viewProjection = m_MVPMatrix;
        ubo.transform = glm::mat4(1.0f);

        cmd.ubo = ubo;
        m_DrawCmds.push_back(cmd);
    }

    void SDL3GPURenderer::DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle)
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

        // Triangle 1
        m_Vertices.push_back({{x1, y0, 0.0f}, {u1, v0}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y0, 0.0f}, {u0, v0}, {1,1,1,1}});
        m_Vertices.push_back({{x1, y1, 0.0f}, {u1, v1}, {1,1,1,1}});
        // Triangle 2
        m_Vertices.push_back({{x1, y1, 0.0f}, {u1, v1}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y1, 0.0f}, {u0, v1}, {1,1,1,1}});
        m_Vertices.push_back({{x0, y0, 0.0f}, {u0, v0}, {1,1,1,1}});

        BatchDrawCmd cmd {};
        cmd.texture = handle;
        cmd.vertexCount = 6;
        cmd.is3D = false;

        UniformBufferObject ubo {};
        ubo.viewProjection = m_MVPMatrix;
        ubo.transform = glm::mat4(1.0f);

        cmd.ubo = ubo;
        m_DrawCmds.push_back(cmd);
    }

    Ref<Texture> SDL3GPURenderer::CreateTexture(const std::string& filePath)
    {
        SDL_Surface* loadedSurf = SDL_LoadPNG(filePath.c_str());
        if (!loadedSurf) return nullptr;

        SDL_Surface* surf = SDL_ConvertSurface(loadedSurf, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(loadedSurf);
        if (!surf) return nullptr;

        SDL_GPUTransferBuffer* transfer_buffer =
            SDL3GPURendererSupport::CreateTextureTransferBuffer(m_Device, surf->w, surf->h);

        SDL_GPUTextureCreateInfo info {};
        info.type        = SDL_GPU_TEXTURETYPE_2D;
        info.format      = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        info.width       = static_cast<Uint32>(surf->w);
        info.height      = static_cast<Uint32>(surf->h);
        info.num_levels  = 1;
        info.usage       = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        info.layer_count_or_depth = 1;

        SDL_GPUTexture* gpuTex = SDL_CreateGPUTexture(m_Device, &info);
        if (!gpuTex)
        {
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            SDL_DestroySurface(surf);
            return nullptr;
        }

        Uint32 pixelSize = surf->w * surf->h * 4;
        void* mapped = SDL_MapGPUTransferBuffer(m_Device, transfer_buffer, false);
        if (!mapped)
        {
            AZ_CORE_ERROR("Failed to map gpu transfer buffer: {0}", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            SDL_DestroySurface(surf);
            return nullptr;
        }
        memcpy(mapped, surf->pixels, pixelSize);
        SDL_UnmapGPUTransferBuffer(m_Device, transfer_buffer);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_Device);
        if (!cmd)
        {
            AZ_CORE_ERROR("Failed to acquire gpu command buffer: {0}", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            SDL_DestroySurface(surf);
            return nullptr;
        }
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTextureTransferInfo transferInfo {};
        transferInfo.transfer_buffer = transfer_buffer;
        transferInfo.offset = 0;

        SDL_GPUTextureRegion region {};
        region.texture = gpuTex;
        region.w = surf->w;
        region.h = surf->h;
        region.d = 1;

        SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);

        uint32_t w = surf->w, h = surf->h;
        SDL_DestroySurface(surf);
        SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);

        return CreateRef<GPUTexture>(m_Device, gpuTex, info.format, w, h);
    }

    Ref<Texture> SDL3GPURenderer::CreateTexture(void* pixels, uint32_t width, uint32_t height)
    {
        SDL_GPUTransferBuffer* transfer_buffer =
            SDL3GPURendererSupport::CreateTextureTransferBuffer(m_Device, width, height);

        SDL_GPUTextureCreateInfo info {};
        info.type        = SDL_GPU_TEXTURETYPE_2D;
        info.format      = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        info.width       = width;
        info.height      = height;
        info.num_levels  = 1;
        info.usage       = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        info.layer_count_or_depth = 1;

        SDL_GPUTexture* gpuTex = SDL_CreateGPUTexture(m_Device, &info);
        if (!gpuTex)
        {
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            AZ_CORE_ERROR("Failed to create GPU texture: {0}", SDL_GetError());
            return nullptr;
        }

        const Uint32 pixelSize = width * height * 4;
        void* mapped = SDL_MapGPUTransferBuffer(m_Device, transfer_buffer, false);
        if (!mapped)
        {
            AZ_CORE_ERROR("Failed to map GPU texture: {0}", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_Device, gpuTex);
            return nullptr;
        }
        memcpy(mapped, pixels, pixelSize);
        SDL_UnmapGPUTransferBuffer(m_Device, transfer_buffer);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_Device);
        if (!cmd)
        {
            AZ_CORE_ERROR("Failed to create GPU command buffer: {0}", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);
            SDL_ReleaseGPUTexture(m_Device, gpuTex);
            return nullptr;
        }
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

        SDL_GPUTextureTransferInfo transferInfo {};
        transferInfo.transfer_buffer = transfer_buffer;
        transferInfo.offset = 0;

        SDL_GPUTextureRegion region {};
        region.texture = gpuTex;
        region.w = width;
        region.h = height;
        region.d = 1;

        SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(m_Device, transfer_buffer);

        return CreateRef<GPUTexture>(m_Device, gpuTex, info.format, width, height);
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

        auto* whiteTex = static_cast<SDL_GPUTexture*>(m_WhiteTexture->GetHandle());

        UniformBufferObject ubo {};
        ubo.viewProjection = m_MVPMatrix;
        ubo.transform = model;

        for (int f = 0; f < 6; ++f)
        {
            const float* c = faceColors[f];

            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][1].x, cubeFaces[f][1].y, cubeFaces[f][1].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][2].x, cubeFaces[f][2].y, cubeFaces[f][2].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][3].x, cubeFaces[f][3].y, cubeFaces[f][3].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});
            m_Vertices.push_back({{cubeFaces[f][0].x, cubeFaces[f][0].y, cubeFaces[f][0].z}, {0, 0}, {c[0], c[1], c[2], c[3]}});

            BatchDrawCmd cmd {};
            cmd.texture = whiteTex;
            cmd.vertexCount = 6;
            cmd.is3D = true;
            cmd.ubo = ubo;
            m_DrawCmds.push_back(cmd);
        }
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
