//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_RENDERER_H
#define AZER_RENDERER_H

#include "Base.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include "SDL3/SDL.h"

namespace azer
{
    struct CameraConfig
    {
        float x = 0, y = 0, z = 0;
        float zoom = 1.0f;
        float fov = 45.0f;
        bool perspective = false;
    };

    class Renderer {
    public:
        virtual ~Renderer() = default;
        virtual bool Initialize(SDL_Window* window) = 0;
        virtual void BeginFrame(const glm::vec3& clearColor) = 0;
        virtual void EndFrame() = 0;
        virtual void SetCamera(const CameraConfig& config) = 0;

        // Renderer2D
        virtual void DrawQuad(float x, float y, float w, float h) = 0;
        virtual void DrawColorQuad(float x, float y, float w, float h, const glm::vec4& color) = 0;
        virtual void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst, float angle = 0.0f) = 0;

        // 这一步本质上是把绘制数据传给GPU，发生在ImGui渲染End时，真正的提交在渲染器End时
        virtual void SetImGuiDrawData(ImDrawData* drawData) = 0;

        virtual Scope<Texture> CreateTexture(const std::string& filePath) = 0;
    };
}

#endif //AZER_RENDERER_H
