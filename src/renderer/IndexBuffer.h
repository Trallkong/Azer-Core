//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include "Mesh2D.h"

namespace Azer
{
    // 前端索引缓冲抽象：Create 按当前后端分派（与 Texture::Create 同模式）。
    class IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        static Ref<IndexBuffer> Create(uint32_t size);

        virtual void Upload(const Indices& indices) = 0;
        virtual uint32_t GetIndexCount() const = 0;
    };
}
