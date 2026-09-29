//
// Created by csis on 2026/9/29.
//
#pragma once

#include "Base.h"
#include "Node2D.h"

namespace Azer
{
    class Texture;

    class Sprite2D : public Node2D
    {
    public:
        explicit Sprite2D(std::string name)
            : Node2D(std::move(name)) {}
        ~Sprite2D() override = default;

        void Draw() override;

        void SetTexture(const Ref<Texture>& texture) { m_Texture = texture; }
    private:
        Ref<Texture> m_Texture;
    };
}



