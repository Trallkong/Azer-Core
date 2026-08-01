#include "azpch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"
#include "UniformBufferData.h"
#include "Mesh2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Azer
{
    Ref<Shader> Renderer2D::s_Shader;
    Ref<VertexBuffer> Renderer2D::s_QuadVbo;
    Ref<IndexBuffer> Renderer2D::s_QuadIbo;
    Ref<Texture> Renderer2D::s_WhiteTexture;

    void Renderer2D::Init()
    {
        s_Shader = Shader::Create("quad2d");

        // 单位四边形（-0.5..0.5），大小由 model 矩阵的缩放决定
        Vertices quadVerts = {
            { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            { { 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            { { 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
            { {-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} },
        };
        Indices quadIdx = { 0, 1, 2, 2, 3, 0 };

        s_QuadVbo = VertexBuffer::Create(static_cast<uint32_t>(quadVerts.size() * sizeof(VertexData)));
        s_QuadVbo->Upload(quadVerts);
        s_QuadIbo = IndexBuffer::Create(static_cast<uint32_t>(quadIdx.size() * sizeof(uint32_t)));
        s_QuadIbo->Upload(quadIdx);

        uint32_t whitePixel = 0xFFFFFFFF;
        s_WhiteTexture = Texture::Create(&whitePixel, 1, 1);
    }

    void Renderer2D::Shutdown()
    {
        s_WhiteTexture.reset();
        s_QuadIbo.reset();
        s_QuadVbo.reset();
        s_Shader.reset();
    }

    void Renderer2D::SetCamera(Camera& camera)
    {
        BufferData data;
        data.viewProjMat = camera.GetViewProjectionMatrix();
        s_Shader->SetUniform("camera", &data, sizeof(data));
    }

    void Renderer2D::DrawQuad(const Transform2D& transform, float alpha)
    {
        DrawColorQuad(transform, { 1.0f, 1.0f, 1.0f, alpha });
    }

    void Renderer2D::DrawColorQuad(const Transform2D& transform, const glm::vec4& color)
    {
        DrawPushConstants pc;
        pc.modelMat = transform.GetMatrix();
        pc.color = color;
        s_Shader->SetUniform("drawData", &pc, sizeof(pc));
        s_WhiteTexture->Bind(1, s_Shader);
        RenderCommand::DrawIndexed(s_QuadVbo, s_QuadIbo, s_Shader);
    }

    void Renderer2D::DrawTexture(const Ref<Texture>& tex, const Transform2D& transform, float alpha)
    {
        // 单位四边形按纹理像素尺寸缩放（1 像素 = 1 单位），再叠加 transform 的缩放
        glm::mat4 model = transform.GetMatrix();
        model = glm::scale(model, glm::vec3(
            static_cast<float>(tex->GetWidth()),
            static_cast<float>(tex->GetHeight()),
            1.0f));

        DrawPushConstants pc;
        pc.modelMat = model;
        pc.color = { 1.0f, 1.0f, 1.0f, alpha };
        s_Shader->SetUniform("drawData", &pc, sizeof(pc));
        tex->Bind(1, s_Shader);
        RenderCommand::DrawIndexed(s_QuadVbo, s_QuadIbo, s_Shader);
    }
}
