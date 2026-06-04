//
// Created by opencode on 2026/6/4.
//

#include "azpch.h"
#include "ECSSceneSerializer.h"
#include "Components.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

namespace azer
{
    using json = nlohmann::json;

    // 组件序列化函数
    static void SerializeIDComponent(json& j, const IDComponent& component)
    {
        j["ID"] = component.ID;
    }

    static void SerializeNameComponent(json& j, const NameComponent& component)
    {
        j["Name"] = component.Name;
    }

    static void SerializeTransformComponent(json& j, const TransformComponent& component)
    {
        j["Position"] = {component.Transform.Position.x, component.Transform.Position.y, component.Transform.Position.z};
        j["Rotation"] = {component.Transform.Rotation.x, component.Transform.Rotation.y, component.Transform.Rotation.z};
        j["Scale"] = {component.Transform.Scale.x, component.Transform.Scale.y, component.Transform.Scale.z};
    }

    static void SerializeRenderComponent(json& j, const RenderComponent& component, const std::string& assetsRoot)
    {
        j["Size"] = {component.Size.x, component.Size.y, component.Size.z};
        j["Color"] = {component.Color.r, component.Color.g, component.Color.b, component.Color.a};
        j["Visible"] = component.Visible;
        
        // 保存相对路径
        if (!component.TexturePath.empty())
        {
            // 这里可以计算相对路径，暂时保存完整路径
            j["TexturePath"] = component.TexturePath;
        }
    }

    static void SerializePhysicsComponent(json& j, const PhysicsComponent& component)
    {
        j["Velocity"] = {component.Velocity.x, component.Velocity.y, component.Velocity.z};
        j["Acceleration"] = {component.Acceleration.x, component.Acceleration.y, component.Acceleration.z};
        j["Mass"] = component.Mass;
        j["IsStatic"] = component.IsStatic;
    }

    static void SerializeCollisionComponent(json& j, const CollisionComponent& component)
    {
        j["ColliderType"] = static_cast<int>(component.ColliderType);
        j["Size"] = {component.Size.x, component.Size.y, component.Size.z};
        j["IsTrigger"] = component.IsTrigger;
    }

    static void SerializeTagComponent(json& j, const TagComponent& component)
    {
        j["Tag"] = component.Tag;
    }

    static void SerializeCameraComponent(json& j, const CameraComponent& component)
    {
        j["CameraType"] = static_cast<int>(component.CameraType);
        j["FOV"] = component.FOV;
        j["NearPlane"] = component.NearPlane;
        j["FarPlane"] = component.FarPlane;
        j["OrthoSize"] = component.OrthoSize;
        j["IsPrimary"] = component.IsPrimary;
    }

    static void SerializeLightComponent(json& j, const LightComponent& component)
    {
        j["LightType"] = static_cast<int>(component.LightType);
        j["Color"] = {component.Color.r, component.Color.g, component.Color.b};
        j["Intensity"] = component.Intensity;
        j["Range"] = component.Range;
        j["SpotAngle"] = component.SpotAngle;
    }

    // 组件反序列化函数
    static void DeserializeIDComponent(const json& j, IDComponent& component)
    {
        component.ID = j["ID"].get<uint64_t>();
    }

    static void DeserializeNameComponent(const json& j, NameComponent& component)
    {
        component.Name = j["Name"].get<std::string>();
    }

    static void DeserializeTransformComponent(const json& j, TransformComponent& component)
    {
        auto pos = j["Position"];
        component.Transform.Position = glm::vec3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
        
        auto rot = j["Rotation"];
        component.Transform.Rotation = glm::vec3(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>());
        
        auto scale = j["Scale"];
        component.Transform.Scale = glm::vec3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
    }

    static void DeserializeRenderComponent(const json& j, RenderComponent& component, const std::string& assetsRoot, Renderer& renderer)
    {
        auto size = j["Size"];
        component.Size = glm::vec3(size[0].get<float>(), size[1].get<float>(), size[2].get<float>());
        
        auto color = j["Color"];
        component.Color = glm::vec4(color[0].get<float>(), color[1].get<float>(), color[2].get<float>(), color[3].get<float>());
        
        component.Visible = j["Visible"].get<bool>();
        
        if (j.contains("TexturePath"))
        {
            component.TexturePath = j["TexturePath"].get<std::string>();
            if (!component.TexturePath.empty())
            {
                // 加载纹理
                std::string fullPath = assetsRoot + "/" + component.TexturePath;
                component.Texture = Texture::Create(renderer, fullPath);
            }
        }
    }

    static void DeserializePhysicsComponent(const json& j, PhysicsComponent& component)
    {
        auto vel = j["Velocity"];
        component.Velocity = glm::vec3(vel[0].get<float>(), vel[1].get<float>(), vel[2].get<float>());
        
        auto acc = j["Acceleration"];
        component.Acceleration = glm::vec3(acc[0].get<float>(), acc[1].get<float>(), acc[2].get<float>());
        
        component.Mass = j["Mass"].get<float>();
        component.IsStatic = j["IsStatic"].get<bool>();
    }

    static void DeserializeCollisionComponent(const json& j, CollisionComponent& component)
    {
        component.ColliderType = static_cast<CollisionComponent::Type>(j["ColliderType"].get<int>());
        
        auto size = j["Size"];
        component.Size = glm::vec3(size[0].get<float>(), size[1].get<float>(), size[2].get<float>());
        
        component.IsTrigger = j["IsTrigger"].get<bool>();
    }

    static void DeserializeTagComponent(const json& j, TagComponent& component)
    {
        component.Tag = j["Tag"].get<std::string>();
    }

    static void DeserializeCameraComponent(const json& j, CameraComponent& component)
    {
        component.CameraType = static_cast<CameraComponent::Type>(j["CameraType"].get<int>());
        component.FOV = j["FOV"].get<float>();
        component.NearPlane = j["NearPlane"].get<float>();
        component.FarPlane = j["FarPlane"].get<float>();
        component.OrthoSize = j["OrthoSize"].get<float>();
        component.IsPrimary = j["IsPrimary"].get<bool>();
    }

    static void DeserializeLightComponent(const json& j, LightComponent& component)
    {
        component.LightType = static_cast<LightComponent::Type>(j["LightType"].get<int>());
        
        auto color = j["Color"];
        component.Color = glm::vec3(color[0].get<float>(), color[1].get<float>(), color[2].get<float>());
        
        component.Intensity = j["Intensity"].get<float>();
        component.Range = j["Range"].get<float>();
        component.SpotAngle = j["SpotAngle"].get<float>();
    }

    bool ECSSceneSerializer::Save(const ECSScene& scene, const std::string& filepath,
                                  const std::string& assetsRoot)
    {
        json sceneJson;
        sceneJson["Entities"] = json::array();

        const auto& entities = scene.GetEntities();
        const World& world = scene.GetWorld();

        for (entt::entity entity : entities)
        {
            json entityJson;
            entityJson["Components"] = json::object();

            // 序列化各个组件
            if (world.HasComponent<IDComponent>(entity))
            {
                SerializeIDComponent(entityJson["Components"]["IDComponent"], world.GetComponent<IDComponent>(entity));
            }

            if (world.HasComponent<NameComponent>(entity))
            {
                SerializeNameComponent(entityJson["Components"]["NameComponent"], world.GetComponent<NameComponent>(entity));
            }

            if (world.HasComponent<TransformComponent>(entity))
            {
                SerializeTransformComponent(entityJson["Components"]["TransformComponent"], world.GetComponent<TransformComponent>(entity));
            }

            if (world.HasComponent<RenderComponent>(entity))
            {
                SerializeRenderComponent(entityJson["Components"]["RenderComponent"], world.GetComponent<RenderComponent>(entity), assetsRoot);
            }

            if (world.HasComponent<PhysicsComponent>(entity))
            {
                SerializePhysicsComponent(entityJson["Components"]["PhysicsComponent"], world.GetComponent<PhysicsComponent>(entity));
            }

            if (world.HasComponent<CollisionComponent>(entity))
            {
                SerializeCollisionComponent(entityJson["Components"]["CollisionComponent"], world.GetComponent<CollisionComponent>(entity));
            }

            if (world.HasComponent<TagComponent>(entity))
            {
                SerializeTagComponent(entityJson["Components"]["TagComponent"], world.GetComponent<TagComponent>(entity));
            }

            if (world.HasComponent<CameraComponent>(entity))
            {
                SerializeCameraComponent(entityJson["Components"]["CameraComponent"], world.GetComponent<CameraComponent>(entity));
            }

            if (world.HasComponent<LightComponent>(entity))
            {
                SerializeLightComponent(entityJson["Components"]["LightComponent"], world.GetComponent<LightComponent>(entity));
            }

            sceneJson["Entities"].push_back(entityJson);
        }

        // 写入文件
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("Failed to open file for writing: {}", filepath);
            return false;
        }

        file << sceneJson.dump(4);
        file.close();

        AZ_CORE_INFO("ECS scene saved to: {}", filepath);
        return true;
    }

    bool ECSSceneSerializer::Load(ECSScene& scene, const std::string& filepath,
                                  const std::string& assetsRoot, Renderer& renderer)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            AZ_CORE_ERROR("Failed to open file for reading: {}", filepath);
            return false;
        }

        json sceneJson;
        try
        {
            file >> sceneJson;
        }
        catch (const json::parse_error& e)
        {
            AZ_CORE_ERROR("Failed to parse JSON file: {}", e.what());
            return false;
        }
        file.close();

        // 清空当前场景
        scene.Clear();

        // 加载实体
        if (sceneJson.contains("Entities"))
        {
            for (const auto& entityJson : sceneJson["Entities"])
            {
                entt::entity entity = scene.CreateEntity();

                if (entityJson.contains("Components"))
                {
                    const auto& components = entityJson["Components"];

                    // 反序列化各个组件
                    if (components.contains("IDComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<IDComponent>(entity);
                        DeserializeIDComponent(components["IDComponent"], component);
                    }

                    if (components.contains("NameComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<NameComponent>(entity);
                        DeserializeNameComponent(components["NameComponent"], component);
                    }

                    if (components.contains("TransformComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<TransformComponent>(entity);
                        DeserializeTransformComponent(components["TransformComponent"], component);
                    }

                    if (components.contains("RenderComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<RenderComponent>(entity);
                        DeserializeRenderComponent(components["RenderComponent"], component, assetsRoot, renderer);
                    }

                    if (components.contains("PhysicsComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<PhysicsComponent>(entity);
                        DeserializePhysicsComponent(components["PhysicsComponent"], component);
                    }

                    if (components.contains("CollisionComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<CollisionComponent>(entity);
                        DeserializeCollisionComponent(components["CollisionComponent"], component);
                    }

                    if (components.contains("TagComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<TagComponent>(entity);
                        DeserializeTagComponent(components["TagComponent"], component);
                    }

                    if (components.contains("CameraComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<CameraComponent>(entity);
                        DeserializeCameraComponent(components["CameraComponent"], component);
                    }

                    if (components.contains("LightComponent"))
                    {
                        auto& component = scene.GetWorld().AddComponent<LightComponent>(entity);
                        DeserializeLightComponent(components["LightComponent"], component);
                    }
                }
            }
        }

        AZ_CORE_INFO("ECS scene loaded from: {}", filepath);
        return true;
    }
}