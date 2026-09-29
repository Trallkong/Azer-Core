//
// Created by csis on 2026/9/29.
//

#include "azpch.h"
#include "Sprite2D.h"
#include "Renderer2D.h"

void Azer::Sprite2D::Draw()
{
    Node2D::Draw();

    if (m_Texture != nullptr)
    {
        Renderer2D::DrawTexture(m_Texture, Transform);
    }
}
