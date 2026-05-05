//
// Created by Trallkong on 2026/5/5.
//

#include "SDL3GPURendererSupport.h"

#include "azpch.h"
#include "SDL3GPURenderer.h"

#include "vert_spv.h"
#include "frag_spv.h"

namespace azer
{
    void SDL3GPURendererSupport::CreateGraphicsPipeline(SDL3GPURenderer* renderer)
    {
        // 顶点着色器
        SDL_GPUShaderCreateInfo vertInfo {};
        vertInfo.code        = vert_spv;
        vertInfo.code_size   = vert_spv_len;
        vertInfo.format      = SDL_GPU_SHADERFORMAT_SPIRV;
        vertInfo.entrypoint  = "main";
        vertInfo.stage       = SDL_GPU_SHADERSTAGE_VERTEX;
        vertInfo.num_uniform_buffers = 1;
        vertInfo.num_samplers        = 0;
        renderer->m_VertexShader = SDL_CreateGPUShader(renderer->m_Device, &vertInfo);

        // 片段着色器
        SDL_GPUShaderCreateInfo fragInfo {};
        fragInfo.code        = frag_spv;
        fragInfo.code_size   = frag_spv_len;
        fragInfo.format      = SDL_GPU_SHADERFORMAT_SPIRV;
        fragInfo.entrypoint  = "main";
        fragInfo.stage       = SDL_GPU_SHADERSTAGE_FRAGMENT;
        fragInfo.num_uniform_buffers = 0;
        fragInfo.num_samplers        = 1;
        renderer->m_FragmentShader = SDL_CreateGPUShader(renderer->m_Device, &fragInfo);

        // 顶点布局
        SDL_GPUVertexBufferDescription vtxBufDesc {};
        vtxBufDesc.slot       = 0;
        vtxBufDesc.pitch      = sizeof(SDL3GPURenderer::BatchVertex);
        vtxBufDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vtxBufDesc.instance_step_rate = 0;

        SDL_GPUVertexAttribute attribs[4] = {};
        // location 0: aPos (vec2)
        attribs[0].location    = 0;
        attribs[0].buffer_slot = 0;
        attribs[0].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attribs[0].offset      = offsetof(SDL3GPURenderer::BatchVertex, pos);
        // location 1: aTexCoord (vec2)
        attribs[1].location    = 1;
        attribs[1].buffer_slot = 0;
        attribs[1].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        attribs[1].offset      = offsetof(SDL3GPURenderer::BatchVertex, texCoord);
        // location 2: aColor (vec4)
        attribs[2].location    = 2;
        attribs[2].buffer_slot = 0;
        attribs[2].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
        attribs[2].offset      = offsetof(SDL3GPURenderer::BatchVertex, color);

        SDL_GPUVertexInputState vtxInput {};
        vtxInput.num_vertex_buffers    = 1;
        vtxInput.vertex_buffer_descriptions = &vtxBufDesc;
        vtxInput.num_vertex_attributes = 3;
        vtxInput.vertex_attributes     = attribs;

        // 混合
        SDL_GPUColorTargetBlendState blend {};
        blend.enable_blend           = true;
        blend.src_color_blendfactor  = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        blend.dst_color_blendfactor  = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        blend.color_blend_op         = SDL_GPU_BLENDOP_ADD;
        blend.src_alpha_blendfactor  = SDL_GPU_BLENDFACTOR_ONE;
        blend.dst_alpha_blendfactor  = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        blend.alpha_blend_op         = SDL_GPU_BLENDOP_ADD;

        SDL_GPUColorTargetDescription colorTargetDesc {};
        colorTargetDesc.format      = SDL_GetGPUSwapchainTextureFormat(renderer->m_Device, renderer->m_Window);
        colorTargetDesc.blend_state = blend;

        SDL_GPUGraphicsPipelineCreateInfo pipeInfo {};
        pipeInfo.vertex_shader    = renderer->m_VertexShader;
        pipeInfo.fragment_shader  = renderer->m_FragmentShader;
        pipeInfo.vertex_input_state = vtxInput;
        pipeInfo.primitive_type   = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipeInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        pipeInfo.target_info.num_color_targets = 1;
        pipeInfo.target_info.color_target_descriptions = &colorTargetDesc;

        renderer->m_Pipeline = SDL_CreateGPUGraphicsPipeline(renderer->m_Device, &pipeInfo);
    }

    void SDL3GPURendererSupport::CreateSampler(SDL3GPURenderer* renderer)
    {
        SDL_GPUSamplerCreateInfo info {};
        info.min_filter    = SDL_GPU_FILTER_LINEAR;
        info.mag_filter    = SDL_GPU_FILTER_LINEAR;
        info.mipmap_mode   = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        renderer->m_Sampler = SDL_CreateGPUSampler(renderer->m_Device, &info);
        if (!renderer->m_Sampler) AZ_CORE_ERROR("Failed to create sampler: {0}", SDL_GetError());
    }

    void SDL3GPURendererSupport::CreateVerticesTransferBuffer(SDL3GPURenderer* renderer)
    {
        // 创建 VertexTransferBuffer
        SDL_GPUTransferBufferCreateInfo vtbInfo {};
        vtbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        vtbInfo.size = renderer->m_MaxVertices * sizeof(SDL3GPURenderer::BatchVertex);
        renderer->m_VerticesTransferBuffer = SDL_CreateGPUTransferBuffer(renderer->m_Device, &vtbInfo);
        if (renderer->m_VerticesTransferBuffer == nullptr)
        {
            AZ_CORE_ERROR("Failed to create vertices buffer: {0}", SDL_GetError());
            assert(false);
        }
    }

    void SDL3GPURendererSupport::CreateWhiteTexture(SDL3GPURenderer* renderer)
    {
        uint8_t white[4] = {255, 255, 255, 255};
        renderer->m_WhiteTexture = renderer->CreateTexture(white, 1, 1);
    }

    void SDL3GPURendererSupport::CreateBuffers(SDL3GPURenderer* renderer)
    {
        SDL_GPUBufferCreateInfo vtxInfo {};
        vtxInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vtxInfo.size  = renderer->m_MaxVertices * sizeof(SDL3GPURenderer::BatchVertex);
        renderer->m_VertexBuffer = SDL_CreateGPUBuffer(renderer->m_Device, &vtxInfo);
        if (renderer->m_VertexBuffer == nullptr)
        {
            AZ_CORE_ERROR("Failed to create vertex buffer: {0}", SDL_GetError());
            assert(false);
        }
    }

    bool SDL3GPURendererSupport::PrepareDrawData(SDL3GPURenderer* renderer, SDL_GPUCommandBuffer* cmd)
    {
        // === Copy Pass: upload vertex data ===
        if (!renderer->m_Vertices.empty())
        {
            const Uint32 vertSize = static_cast<Uint32>(renderer->m_Vertices.size()) * sizeof(SDL3GPURenderer::BatchVertex);

            void* mapped = SDL_MapGPUTransferBuffer(renderer->m_Device, renderer->m_VerticesTransferBuffer, true);
            if (!mapped) {
                renderer->m_Vertices.clear();
                renderer->m_DrawCmds.clear();
                SDL_SubmitGPUCommandBuffer(cmd);
                renderer->m_ImGuiDrawData = nullptr;
                return false;
            }
            memcpy(mapped, renderer->m_Vertices.data(), vertSize);
            SDL_UnmapGPUTransferBuffer(renderer->m_Device, renderer->m_VerticesTransferBuffer);

            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

            SDL_GPUTransferBufferLocation src {};
            src.transfer_buffer = renderer->m_VerticesTransferBuffer;
            src.offset = 0;

            SDL_GPUBufferRegion dstVtx {};
            dstVtx.buffer = renderer->m_VertexBuffer;
            dstVtx.offset = 0;
            dstVtx.size = vertSize;
            SDL_UploadToGPUBuffer(copyPass, &src, &dstVtx, true);
            SDL_EndGPUCopyPass(copyPass);
        }
        return true;
    }

    SDL_GPUTransferBuffer* SDL3GPURendererSupport::CreateTextureTransferBuffer(SDL_GPUDevice* device, uint32_t width, uint32_t height)
    {
        // 创建 TextureTransferBuffer
        SDL_GPUTransferBufferCreateInfo ttbInfo {};
        ttbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        ttbInfo.size = 4 * width * height;
        SDL_GPUTransferBuffer* buffer = SDL_CreateGPUTransferBuffer(device, &ttbInfo);
        if (buffer == nullptr)
        {
            AZ_CORE_ERROR("Failed to create transfer buffer: {0}", SDL_GetError());
            return nullptr;
        }
        return buffer;
    }
} // azer