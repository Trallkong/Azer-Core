//
// Created by Trallkong on 2026/5/1.
//

#pragma once

// Base
#include "base/Base.h"
#include "base/Variant.h"
#include "base/Application.h"
#include "base/Logger.h"
#include "base/event/Event.h"
#include "base/Layer.h"
#include "base/Input.h"
#include "base/Random.h"
#include "base/Window.h"
#include "base/Transform2D.h"
#include "base/Transform3D.h"
#include "base/GameObject.h"
#include "base/Scene.h"
#include "base/SceneSerializer.h"
#include "base/Collision.h"
#include "base/file_system/FileSystem.h"

// Resource
#include "resources/Resource.h"
#include "resources/SkyBox.h"

// Animation
#include "base/animation/AnimationPlayer.h"

// Reflection
#include "base/reflection/PropertyAccessor.h"

// Renderer
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Mesh2D.h"
#include "renderer/Framebuffer.h"
#include "renderer/RendererAPI.h"
#include "renderer/Renderer.h"
#include "renderer/RenderCommand.h"
#include "renderer/Renderer2D.h"
#include "renderer/Renderer3D.h"
#include "renderer/Camera.h"
#include "renderer/Camera2D.h"
#include "renderer/Camera3D.h"
#include "renderer/Mesh.h"
#include "renderer/Material.h"
#include "renderer/Model.h"

// ECS
#include "ecs/Components.h"
#include "ecs/World.h"
#include "ecs/System.h"
#include "ecs/SystemManager.h"
#include "ecs/ECSLayer.h"
#include "ecs/ECSScene.h"
#include "ecs/ECSSceneSerializer.h"
#include "ecs/GameObjectWrapper.h"
#include "ecs/RenderSystem.h"
#include "ecs/PhysicsSystem.h"
