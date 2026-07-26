//
// Created by Aier on 2026/6/1.
//

#pragma once
#include "Scene.h"
#include "Renderer.h"
#include <string>

namespace Azer
{
    class SceneSerializer
    {
    public:
        // 保存场景到 JSON 文件
        // assetsRoot: 纹理路径的相对基准目录
        static bool Save(const Scene& scene, const std::string& filepath,
                         const std::string& assetsRoot);

        // 从 JSON 文件加载场景
        static bool Load(Scene& scene, const std::string& filepath,
                         const std::string& assetsRoot, Renderer& renderer);
    };
}
