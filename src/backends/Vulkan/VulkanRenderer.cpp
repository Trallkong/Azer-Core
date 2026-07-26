#include "azpch.h"
#include "VulkanRenderer.h"

namespace Azer {

    VulkanRenderer::~VulkanRenderer()
    {
    }

    bool VulkanRenderer::Initialize(Window *window)
    {
        m_Context.Init(window);
        return true;
    }
    
    void VulkanRenderer::BeginFrame(const glm::vec3 &clearColor)
    {
        AZ_ASSERT(false, "VulkanRenderer::BeginFrame not implemented yet");
    }
    
    void VulkanRenderer::EndFrame()
    {
        AZ_ASSERT(false, "VulkanRenderer::EndFrame not implemented yet");
    }

    void VulkanRenderer::SetCamera(Camera &camera)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetCamera not implemented yet");
    }

    void VulkanRenderer::ResetRenderState()
    {
        AZ_ASSERT(false, "VulkanRenderer::ResetRenderState not implemented yet");
    }

    void VulkanRenderer::SetRenderTarget(Framebuffer *target)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetRenderTarget not implemented yet");
    }

    void VulkanRenderer::SetViewport(uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetViewport not implemented yet");
    }

    void VulkanRenderer::DrawQuad(float x, float y, float w, float h, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawQuad not implemented yet");
    }

    void VulkanRenderer::DrawColorQuad(float x, float y, float w, float h, const glm::vec4 &color, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawColorQuad not implemented yet");
    }

    void VulkanRenderer::DrawTexture(Texture *tex, const SDL_FRect &src, const SDL_FRect &dst, float angle, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawTexture not implemented yet");
    }

    void VulkanRenderer::DrawCube(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawCube not implemented yet");
    }

    void VulkanRenderer::DrawModel(Model &model, const glm::mat4 &worldTransform, float alpha)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawModel not implemented yet");
    }

    void VulkanRenderer::DrawSkybox(const Ref<Texture> &hdrTexture)
    {
        AZ_ASSERT(false, "VulkanRenderer::DrawSkybox not implemented yet");
    }

    void VulkanRenderer::ImGuiInit(SDL_Window *window)
    {
        AZ_ASSERT(false, "VulkanRenderer::ImGuiInit not implemented yet");
    }

    void VulkanRenderer::ImGuiShutdown()
    {
        AZ_ASSERT(false, "VulkanRenderer::ImGuiShutdown not implemented yet");
    }

    void VulkanRenderer::ImGuiNewFrame()
    {
        AZ_ASSERT(false, "VulkanRenderer::ImGuiNewFrame not implemented yet");
    }

    void VulkanRenderer::SetImGuiDrawData(ImDrawData *drawData)
    {
        AZ_ASSERT(false, "VulkanRenderer::SetImGuiDrawData not implemented yet");
    }

    Ref<Texture> VulkanRenderer::CreateTexture(const std::string &filePath)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Texture> VulkanRenderer::CreateTexture(void *pixels, uint32_t width, uint32_t height)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Texture> VulkanRenderer::CreateHDRTexture(const std::string &filePath)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateHDRTexture not implemented yet");
        return Ref<Texture>();
    }

    Ref<Framebuffer> VulkanRenderer::CreateFramebuffer(const FramebufferSpec &spec)
    {
        AZ_ASSERT(false, "VulkanRenderer::CreateFramebuffer not implemented yet");
        return Ref<Framebuffer>();
    }
}