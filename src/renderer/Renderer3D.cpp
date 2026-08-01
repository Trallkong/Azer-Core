#include "azpch.h"
#include "Renderer3D.h"

#include "RenderCommand.h"
#include "UniformBufferData.h"
#include "Camera3D.h"
#include "Mesh2D.h"

namespace Azer
{
    Ref<Shader> Renderer3D::s_Shader;
    Ref<VertexBuffer> Renderer3D::s_CubeVbo;
    Ref<IndexBuffer> Renderer3D::s_CubeIbo;
    Ref<Texture> Renderer3D::s_WhiteTexture;

    void Renderer3D::Init()
    {
        s_Shader = Shader::Create("base3d");

        // 单位立方体：6 个面，每面 4 个独立顶点（便于每面不同颜色），36 索引
        std::vector<VertexData> vertices(CUBE_VERTEX_COUNT);
        std::vector<uint32_t> indices(CUBE_INDEX_COUNT);

        const float h = 0.5f;
        const glm::vec3 facePos[6][4] = {
            { {-h, -h,  h}, { h, -h,  h}, { h,  h,  h}, {-h,  h,  h} },   // +Z
            { { h, -h, -h}, {-h, -h, -h}, {-h,  h, -h}, { h,  h, -h} },   // -Z
            { { h, -h,  h}, { h, -h, -h}, { h,  h, -h}, { h,  h,  h} },   // +X
            { {-h, -h, -h}, {-h, -h,  h}, {-h,  h,  h}, {-h,  h, -h} },   // -X
            { {-h,  h,  h}, { h,  h,  h}, { h,  h, -h}, {-h,  h, -h} },   // +Y
            { {-h, -h, -h}, { h, -h, -h}, { h, -h,  h}, {-h, -h,  h} },   // -Y
        };
        const glm::vec4 faceColor[6] = {
            {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}, {1, 0, 1, 1}
        };
        const glm::vec2 uv[4] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };

        for (uint32_t f = 0; f < 6; ++f)
        {
            for (uint32_t v = 0; v < 4; ++v)
            {
                uint32_t idx = f * 4 + v;
                vertices[idx].position = facePos[f][v];
                vertices[idx].uv = uv[v];
                vertices[idx].color = faceColor[f];
            }
        }

        uint32_t vi = 0;
        for (uint32_t f = 0; f < 6; ++f)
        {
            uint32_t base = f * 4;
            indices[vi++] = base + 0;
            indices[vi++] = base + 1;
            indices[vi++] = base + 2;
            indices[vi++] = base + 2;
            indices[vi++] = base + 3;
            indices[vi++] = base + 0;
        }

        s_CubeVbo = VertexBuffer::Create(static_cast<uint32_t>(vertices.size() * sizeof(VertexData)));
        s_CubeVbo->Upload(vertices);
        s_CubeIbo = IndexBuffer::Create(static_cast<uint32_t>(indices.size() * sizeof(uint32_t)));
        s_CubeIbo->Upload(indices);

        uint32_t whitePixel = 0xFFFFFFFF;
        s_WhiteTexture = Texture::Create(&whitePixel, 1, 1);
    }

    void Renderer3D::Shutdown()
    {
        s_WhiteTexture.reset();
        s_CubeIbo.reset();
        s_CubeVbo.reset();
        s_Shader.reset();
    }

    void Renderer3D::SetCamera(Camera& camera)
    {
        auto* cam3d = dynamic_cast<Camera3D*>(&camera);
        if (cam3d == nullptr)
        {
            return;
        }

        CameraData data;
        data.position = cam3d->GetTransform().Position;
        data.viewMatrix = cam3d->GetViewMatrix();
        data.projectionMatrix = cam3d->GetProjectionMatrix();
        s_Shader->SetUniform("camera", &data, sizeof(data));
    }

    void Renderer3D::DrawCube(const Transform3D& transform)
    {
        glm::mat4 model = transform.GetMatrix();
        s_Shader->SetUniform("drawData", &model, sizeof(model));
        s_WhiteTexture->Bind(1, s_Shader);
        RenderCommand::DrawIndexed(s_CubeVbo, s_CubeIbo, s_Shader);
    }

    void Renderer3D::DrawMesh(const Ref<VertexBuffer>& vertexBuffer,
                              const Ref<IndexBuffer>& indexBuffer,
                              const Ref<Shader>& shader)
    {
        RenderCommand::DrawIndexed(vertexBuffer, indexBuffer, shader);
    }
}
