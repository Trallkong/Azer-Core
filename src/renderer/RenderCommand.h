//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Azer
{
    class Renderer;

    // 底层绘制命令的静态门面：Draw / DrawIndexed 等静态方法，内部调用静态后端渲染器执行。
    // 高层便利层（Renderer2D/3D）与自定义绘制都经由这里提交绘制命令。
    class RenderCommand
    {
    public:
        static void Init(Renderer* renderer);
        static Renderer* GetRenderer() { return s_Renderer; }

        // 非索引绘制；uniform 由 shader->SetUniform 提供，纹理由 texture->Bind(binding, shader) 绑定
        static void Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                         const Ref<Shader>& shader);

        // 索引绘制
        static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                                const Ref<IndexBuffer>& indexBuffer,
                                const Ref<Shader>& shader);

    private:
        static Renderer* s_Renderer;
    };
}
