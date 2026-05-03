//
// Created by Trallkong on 2026/4/30.
//

#include "azpch.h"
#include "Renderer2D.h"
#include "Application.h"

void azer::Renderer2D::DrawQuad(const float x, const float y, const float w, const float h)
{
    const auto& renderer = Application::Get().GetRenderer();
    renderer->DrawQuad(x, y, w, h);
}

void azer::Renderer2D::DrawColorQuad(const float x, const float y, const float w, const float h, const glm::vec4& color)
{
    const auto& renderer = Application::Get().GetRenderer();
    renderer->DrawColorQuad(x, y, w, h, color);
}

azer::Scope<azer::Texture> azer::Renderer2D::CreateTexture(const std::string& filePath)
{
    const auto& renderer = Application::Get().GetRenderer();
    return renderer->CreateTexture(filePath);
}

void azer::Renderer2D::DrawTexture(Texture* texture, const SDL_FRect& src, const SDL_FRect& dst, float angle)
{
    const auto& renderer = Application::Get().GetRenderer();
    renderer->DrawTexture(texture, src, dst, angle);
}
