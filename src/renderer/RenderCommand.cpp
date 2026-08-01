#include "azpch.h"
#include "RenderCommand.h"

#include "Renderer.h"

namespace Azer
{
    Renderer* RenderCommand::s_Renderer = nullptr;

    void RenderCommand::Init(Renderer* renderer)
    {
        s_Renderer = renderer;
    }

    void RenderCommand::Draw(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount,
                             const Ref<Shader>& shader)
    {
        AZ_ASSERT(s_Renderer != nullptr, "RenderCommand::Draw called before Init");
        if (s_Renderer == nullptr)
        {
            return;
        }
        s_Renderer->Draw(vertexBuffer, vertexCount, shader);
    }

    void RenderCommand::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
                                    const Ref<IndexBuffer>& indexBuffer,
                                    const Ref<Shader>& shader)
    {
        AZ_ASSERT(s_Renderer != nullptr, "RenderCommand::DrawIndexed called before Init");
        if (s_Renderer == nullptr)
        {
            return;
        }
        s_Renderer->DrawIndexed(vertexBuffer, indexBuffer, shader);
    }
}
