//
// Created by opencode on 2026/6/4.
//

#pragma once

#include "ecs/ECSScene.h"
#include "Renderer.h"
#include <string>

namespace azer
{
    class ECSSceneSerializer
    {
    public:
        // 保存ECS场景到 JSON 文件
        // assetsRoot: 纹理路径的相对基准目录
        static bool Save(const ECSScene& scene, const std::string& filepath,
                         const std::string& assetsRoot);

        // 从 JSON 文件加载ECS场景
        static bool Load(ECSScene& scene, const std::string& filepath,
                         const std::string& assetsRoot, Renderer& renderer);
    };
}