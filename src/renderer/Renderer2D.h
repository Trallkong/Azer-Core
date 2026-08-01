//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Transform2D.h"

namespace Azer
{
    // 前端 2D 便利渲染器：内置单位四边形网格 + 默认 quad2d shader。
    // 相机数据通过 shader->SetUniform 上传（每帧缓冲由后端管理），
    // 绘制经 RenderCommand 提交给后端执行。
    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void SetCamera(Camera& camera);
        static void DrawQuad(const Transform2D& transform, float alpha = 1.0f);
        static void DrawColorQuad(const Transform2D& transform, const glm::vec4& color);
        static void DrawTexture(const Ref<Texture>& tex, const Transform2D& transform, float alpha = 1.0f);

    private:
        static Ref<Shader> s_Shader;
        static Ref<VertexBuffer> s_QuadVbo;
        static Ref<IndexBuffer> s_QuadIbo;
        static Ref<Texture> s_WhiteTexture;
    };
}
