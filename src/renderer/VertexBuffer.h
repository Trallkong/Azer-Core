//
// Created by Trallkong on 2026/8/1.
//

#pragma once
#include "Base.h"
#include "Mesh2D.h"

namespace Azer
{
    // 前端顶点缓冲抽象：Create 按当前后端分派（与 Texture::Create 同模式）。
    // Bind 等后端相关操作由后端内部完成，前端只负责创建与上传数据。
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        static Ref<VertexBuffer> Create(uint32_t size);

        virtual void Upload(const Vertices& vertices) = 0;
        virtual uint32_t GetSize() const = 0;
    };
}
