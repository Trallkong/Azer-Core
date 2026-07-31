//
// Created by Aier on 2026/6/1.
//

#include "azpch.h"
#include "SceneSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace Azer
{
    using json = nlohmann::json;

    bool SceneSerializer::Save(const Scene& scene, const std::string& filepath,
                               const std::string& assetsRoot)
    {
        json j;
        json objects = json::array();

        for (const auto& obj : scene.GetObjects())
        {
            json jObj;
            jObj["name"] = obj->GetName();

            const auto& t = obj->GetTransform();
            jObj["transform"] = {
                {"position", {t.Position.x, t.Position.y, t.Position.z}},
                {"rotation", {t.Rotation.x, t.Rotation.y, t.Rotation.z}},
                {"scale",    {t.Scale.x,    t.Scale.y,    t.Scale.z}}}
            ;

            const auto& size = obj->GetSize();
            jObj["size"] = {size.x, size.y, size.z};

            const auto& color = obj->GetColor();
            jObj["color"] = {color.r, color.g, color.b, color.a};

            jObj["visible"] = obj->IsVisible();

            // 纹理路径（相对路径）
            const std::string& texPath = obj->GetTexturePath();
            if (!texPath.empty())
            {
                std::filesystem::path absPath(texPath);
                std::filesystem::path rootPath(assetsRoot);
                std::error_code ec;
                auto rel = std::filesystem::relative(absPath, rootPath, ec);
                jObj["texture"] = ec ? texPath : rel.generic_string();
            }
            else
            {
                jObj["texture"] = "";
            }

            objects.push_back(jObj);
        }

        j["objects"] = objects;

        // 确保目录存在
        std::filesystem::path path(filepath);
        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path());

        std::ofstream file(filepath);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("SceneSerializer: Failed to write {}", filepath);
            return false;
        }

        file << j.dump(4);
        AZ_CORE_INFO("SceneSerializer: Saved {} objects to {}", scene.GetObjectCount(), filepath);
        return true;
    }

    bool SceneSerializer::Load(Scene& scene, const std::string& filepath,
                               const std::string& assetsRoot, Renderer& renderer)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("SceneSerializer: Failed to read {}", filepath);
            return false;
        }

        json j;
        try
        {
            j = json::parse(file);
        }
        catch (const json::parse_error& e)
        {
            AZ_CORE_ERROR("SceneSerializer: JSON parse error: {}", e.what());
            return false;
        }
        file.close();

        scene.Clear();

        if (!j.contains("objects"))
        {
            AZ_CORE_WARN("SceneSerializer: No objects in {}", filepath);
            return true;
        }

        for (const auto& jObj : j["objects"])
        {
            std::string name = jObj.value("name", "GameObject");
            auto& obj = scene.CreateObject(name);

            // Transform
            if (jObj.contains("transform"))
            {
                const auto& jt = jObj["transform"];
                if (jt.contains("position"))
                {
                    auto& p = jt["position"];
                    obj.GetTransform().Position = {p[0].get<float>(), p[1].get<float>(), p[2].get<float>()};
                }
                if (jt.contains("rotation"))
                {
                    auto& r = jt["rotation"];
                    obj.GetTransform().Rotation = {r[0].get<float>(), r[1].get<float>(), r[2].get<float>()};
                }
                if (jt.contains("scale"))
                {
                    auto& s = jt["scale"];
                    obj.GetTransform().Scale = {s[0].get<float>(), s[1].get<float>(), s[2].get<float>()};
                }
            }

            // Size
            if (jObj.contains("size"))
            {
                auto& s = jObj["size"];
                obj.SetSize({s[0].get<float>(), s[1].get<float>(), s[2].get<float>()});
            }

            // Color
            if (jObj.contains("color"))
            {
                auto& c = jObj["color"];
                obj.SetColor({c[0].get<float>(), c[1].get<float>(), c[2].get<float>(), c[3].get<float>()});
            }

            // Visibility
            if (jObj.contains("visible"))
                obj.SetVisible(jObj["visible"].get<bool>());

            // Texture
            if (jObj.contains("texture"))
            {
                std::string texRel = jObj["texture"].get<std::string>();
                if (!texRel.empty())
                {
                    std::filesystem::path absPath = std::filesystem::path(assetsRoot) / texRel;
                    std::string absStr = absPath.string();

                    auto tex = Texture::Create(absStr);
                    if (tex)
                    {
                        obj.SetTexture(tex);
                        obj.SetTexturePath(absStr);
                    }
                    else
                    {
                        AZ_CORE_WARN("SceneSerializer: Failed to load texture {}", absStr);
                    }
                }
            }
        }

        AZ_CORE_INFO("SceneSerializer: Loaded {} objects from {}", scene.GetObjectCount(), filepath);
        return true;
    }
}
