//
// Created by Trallkong on 2026/5/1.
//

#pragma once

// Base
#include "base/Base.h"
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
#include "base/UUID.h"
#include "base/Type.h"

// Resource
#include "resources/Resource.h"
#include "resources/node/Node.h"
#include "resources/node/Node2D.h"
#include "resources/node/Node3D.h"
#include "resources/node/Timer.h"
#include "resources/node/AnimationPlayer.h"
#include "resources/node/node2d/Sprite2D.h"
#include "resources/node/node2d/ui/Button.h"
#include "resources/SkyBox.h"

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
