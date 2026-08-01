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
#include "Transform3D.h"

#include <vector>

namespace Azer
{
    // 前端 3D 便利渲染器：内置立方体网格 + 默认 base3d shader。
    // 相机数据通过 shader->SetUniform 上传（每帧缓冲由后端管理），
    // 绘制经 RenderCommand 提交给后端执行。
    class Renderer3D
    {
    public:
        static void Init();
        static void Shutdown();

        static void SetCamera(Camera& camera);
        static void DrawCube(const Transform3D& transform);

        // 自定义绘制：用户自己的 shader / 缓冲，纹理先用 texture->Bind(binding, shader) 绑定
        static void DrawMesh(const Ref<VertexBuffer>& vertexBuffer,
                             const Ref<IndexBuffer>& indexBuffer,
                             const Ref<Shader>& shader);

        static void DrawSkybox();

    private:
        static constexpr uint32_t CUBE_VERTEX_COUNT = 24;
        static constexpr uint32_t CUBE_INDEX_COUNT = 36;

        static Ref<Shader> s_Shader;
        static Ref<VertexBuffer> s_CubeVbo;
        static Ref<IndexBuffer> s_CubeIbo;
        static Ref<Texture> s_WhiteTexture;
    };
}
