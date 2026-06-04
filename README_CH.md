# Azer-Core

[English](README.md) | [中文](README_CH.md)

一款轻量级、跨平台的 C++23 2D/3D 游戏引擎框架。基于**引擎即库**（engine-as-a-library）模式设计：Azer 编译为静态/动态库，用户应用程序链接该库，支持可切换的渲染后端和分层更新架构——全部基于 SDL3 驱动。

## 什么是 Azer-Core

Azer-Core 是一个现代 C++23 游戏引擎框架，提供：

- **引擎即库**架构 — 非可执行文件；用户定义 `CreateApplication()`，链接 `Azer`
- **可切换的渲染后端** — `Simple2D`（SDL_Renderer）和 `ForwardPlus`（SDL GPU API）
- **实体组件系统（ECS）** — 基于 entt 的灵活实体管理
- **分层更新架构** — 确定性游戏循环，固定时间步物理
- **依赖注入** — 无全局单例；Layer 通过 `OnAttach` 接收 `EngineContext{ Renderer&, Window& }`
- **跨平台** — 通过 SDL3 支持 Windows、Linux、macOS

## 架构概览

```
┌─────────────────────────────────────────────────────────────────┐
│                      应用层                                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │  用户应用   │  │  编辑器     │  │  ECS 示例   │             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
│         │                │                │                     │
│         └────────────────┼────────────────┘                     │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────┐             │
│  │              Azer 引擎核心                     │             │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────┐ │             │
│  │  │   Layer     │  │   ECS       │  │ Event  │ │             │
│  │  │  系统       │  │  系统       │  │ 系统   │ │             │
│  │  └──────┬──────┘  └──────┬──────┘  └────┬───┘ │             │
│  │         │                │              │      │             │
│  │  ┌──────▼────────────────▼──────────────▼───┐ │             │
│  │  │           引擎上下文                      │ │             │
│  │  │        { Renderer&, Window& }             │ │             │
│  │  └───────────────────┬──────────────────────┘ │             │
│  │                      │                        │             │
│  │  ┌───────────────────▼──────────────────────┐ │             │
│  │  │           渲染器抽象层                    │ │             │
│  │  │  ┌─────────────┐      ┌─────────────┐   │ │             │
│  │  │  │ SDL3Renderer│      │SDL3GPURender│   │ │             │
│  │  │  │   (2D)      │      │   (3D)      │   │ │             │
│  │  │  └─────────────┘      └─────────────┘   │ │             │
│  │  └─────────────────────────────────────────┘ │             │
│  └──────────────────────────────────────────────┘             │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────┐             │
│  │              平台层                            │             │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────┐ │             │
│  │  │   SDL3      │  │   GLM       │  │ spdlog │ │             │
│  │  │  (窗口/     │  │  (数学)     │  │(日志)  │ │             │
│  │  │   输入)     │  │             │  │        │ │             │
│  │  └─────────────┘  └─────────────┘  └────────┘ │             │
│  └───────────────────────────────────────────────┘             │
└─────────────────────────────────────────────────────────────────┘
```

### 核心系统交互

```
┌─────────────────────────────────────────────────────────────────┐
│                    游戏循环 (Application::Run)                   │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   事件      │    │   物理      │    │   渲染      │         │
│  │  分发       │───▶│   更新      │───▶│   帧        │         │
│  │ (逆序)      │    │ (固定DT)    │    │             │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │  Layers     │    │  Layers     │    │  Layers     │         │
│  │ OnEvent()   │    │OnPhysics    │    │  OnDraw()   │         │
│  │             │    │  Update()   │    │             │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    ECS 世界                                 ││
│  │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     ││
│  │  │   实体      │    │   组件      │    │   系统      │     ││
│  │  │  (IDs)      │    │  (数据)     │    │ (行为)      │     ││
│  │  └─────────────┘    └─────────────┘    └─────────────┘     ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## 模块设计

### 目录结构

```
Azer/
├── src/
│   ├── Azer.h                    # 总头文件
│   ├── azpch.h                   # 预编译头
│   ├── base/                     # 核心引擎系统
│   │   ├── Application.h/cpp     # 主应用循环，Layer 管理
│   │   ├── Base.h                # AppMode, Ref/Scope/Weak 别名
│   │   ├── EngineContext.h       # 依赖注入上下文
│   │   ├── EntryPoint.h          # main() 入口点
│   │   ├── Layer.h               # Layer 基类
│   │   ├── LayerStack.h/cpp      # Layer 容器
│   │   ├── Logger.h/cpp          # 基于 spdlog 的双日志器
│   │   ├── Input.h/cpp           # 静态键盘输入
│   │   ├── Window.h              # 抽象窗口接口
│   │   ├── GameObject.h/cpp      # 传统实体（遗留）
│   │   ├── Scene.h/cpp           # GameObject 集合
│   │   ├── SceneSerializer.h/cpp # JSON 场景序列化
│   │   ├── Transform2D.h         # 2D 变换
│   │   ├── Transform3D.h         # 3D 变换
│   │   ├── Collision.h           # AABB/球体碰撞
│   │   ├── Variant.h/cpp         # 类型擦除值容器
│   │   ├── event/                # 事件系统
│   │   ├── animation/            # 动画系统
│   │   ├── reflection/           # 属性反射
│   │   └── file_system/          # 文件 I/O
│   ├── ecs/                      # 实体组件系统
│   │   ├── Components.h          # 组件定义
│   │   ├── World.h/cpp           # 实体/组件管理器
│   │   ├── System.h              # 系统基类
│   │   ├── SystemManager.h/cpp   # 系统编排
│   │   ├── ECSLayer.h/cpp        # ECS 集成层
│   │   ├── RenderSystem.h/cpp    # 渲染系统
│   │   ├── PhysicsSystem.h/cpp   # 物理系统
│   │   ├── ECSScene.h/cpp        # ECS 场景管理
│   │   ├── ECSSceneSerializer.h/cpp # ECS 序列化
│   │   └── GameObjectWrapper.h/cpp  # 遗留桥接
│   ├── renderer/                 # 抽象渲染器类型
│   │   ├── Renderer.h/cpp        # 纯虚渲染器
│   │   ├── Camera.h              # 抽象相机
│   │   ├── Camera2D.h            # 2D 相机
│   │   ├── Camera3D.h            # 3D 相机
│   │   ├── Texture.h/cpp         # 抽象纹理
│   │   ├── Framebuffer.h/cpp     # 抽象帧缓冲
│   │   ├── Model.h/cpp           # GLTF 模型加载器
│   │   ├── Mesh.h                # 顶点/网格数据
│   │   └── Material.h            # PBR 材质
│   └── backends/                 # 具体实现
│       ├── SDL3Renderer/         # SDL_Renderer 后端
│       ├── SDL3GPURenderer/      # SDL_GPU 后端
│       └── SDL3Window/           # SDL3 窗口后端
├── vendor/                       # 第三方依赖
│   ├── SDL/                      # SDL3 (git 子模块)
│   ├── glm/                      # GLM 数学库 (git 子模块)
│   ├── spdlog/                   # spdlog 日志 (git 子模块)
│   ├── imgui/                    # Dear ImGui (直接提交)
│   ├── entt/                     # entt ECS (git clone)
│   ├── cgltf/                    # glTF 加载器 (内置)
│   ├── stb/                      # stb_image (内置)
│   └── nlohmann_json/            # JSON 库 (内置)
└── assets/                       # 引擎资源
    └── shaders/                  # GLSL 着色器 + SPIR-V
```

### 模块依赖

```
┌─────────────────────────────────────────────────────────────────┐
│                    依赖关系图                                    │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │  用户应用   │    │   编辑器    │    │ ECS 示例    │         │
│  └──────┬──────┘    └──────┬──────┘    └──────┬──────┘         │
│         │                  │                  │                 │
│         └──────────────────┼──────────────────┘                 │
│                            │                                    │
│  ┌─────────────────────────▼─────────────────────────┐         │
│  │              Azer 引擎库                           │         │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────────┐ │         │
│  │  │   base/     │  │   ecs/      │  │ renderer/  │ │         │
│  │  │  (核心)     │  │  (ECS)      │  │(抽象)      │ │         │
│  │  └──────┬──────┘  └──────┬──────┘  └─────┬──────┘ │         │
│  │         │                │               │        │         │
│  │         └────────────────┼───────────────┘        │         │
│  │                          │                        │         │
│  │  ┌───────────────────────▼──────────────────┐    │         │
│  │  │           backends/ (实现)                │    │         │
│  │  │  ┌─────────────┐  ┌─────────────┐        │    │         │
│  │  │  │SDL3Renderer │  │SDL3GPURender│        │    │         │
│  │  │  └─────────────┘  └─────────────┘        │    │         │
│  │  └─────────────────────────────────────────┘    │         │
│  └──────────────────────────────────────────────────┘         │
│                            │                                    │
│  ┌─────────────────────────▼─────────────────────────┐         │
│  │              vendor/ (依赖)                        │         │
│  │  ┌─────┐ ┌─────┐ ┌───────┐ ┌─────┐ ┌─────┐      │         │
│  │  │ SDL │ │ GLM │ │spdlog │ │ImGui│ │entt │      │         │
│  │  └─────┘ └─────┘ └───────┘ └─────┘ └─────┘      │         │
│  └──────────────────────────────────────────────────┘         │
└─────────────────────────────────────────────────────────────────┘
```

## 数据流

### 应用生命周期

```
┌─────────────────────────────────────────────────────────────────┐
│                应用生命周期                                      │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   创建      │    │   初始化    │    │    运行     │         │
│  │Application()│───▶│  系统       │───▶│  游戏循环   │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                                                   │             │
│                                                   ▼             │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    游戏循环                                 ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 1. 轮询事件 (SDL_PollEvent)                         │   ││
│  │  │    └─▶ 转换为类型化事件                             │   ││
│  │  │    └─▶ 分发给 Layers（逆序）                       │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 2. 固定时间步物理（累加器）                         │   ││
│  │  │    └─▶ 对每个 Layer 执行 OnPhysicsUpdate(fixedDt)   │   ││
│  │  │    └─▶ ECS PhysicsSystem 更新                      │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 3. 帧更新                                          │   ││
│  │  │    └─▶ 对每个 Layer 执行 OnUpdate(dt)              │   ││
│  │  │    └─▶ ECS 系统更新                                │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 4. 插值                                            │   ││
│  │  │    └─▶ OnInterpolate(alpha) 实现平滑渲染           │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 5. 渲染                                            │   ││
│  │  │    └─▶ BeginFrame()                                │   ││
│  │  │    └─▶ 对每个 Layer 执行 OnDraw()                  │   ││
│  │  │    └─▶ ECS RenderSystem                            │   ││
│  │  │    └─▶ 对 ImGui Layer 执行 OnImGuiRender()        │   ││
│  │  │    └─▶ EndFrame()                                  │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### ECS 数据流

```
┌─────────────────────────────────────────────────────────────────┐
│                    ECS 数据流                                    │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   实体      │    │   组件      │    │   系统      │         │
│  │  (IDs)      │───▶│  (数据)     │───▶│ (行为)      │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │  entt       │    │ Transform   │    │ Render      │         │
│  │  registry   │    │ Render      │    │ Physics     │         │
│  │             │    │ Physics     │    │ Collision   │         │
│  │             │    │ Collision   │    │ Animation   │         │
│  │             │    │ ...         │    │ ...         │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                系统执行顺序                                 ││
│  │  1. PhysicsSystem (OnPhysicsUpdate)                         ││
│  │  2. AnimationSystem (OnUpdate)                              ││
│  │  3. CollisionSystem (OnUpdate)                              ││
│  │  4. RenderSystem (OnRender)                                 ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## 关键系统

### 1. Layer 系统

Layer 系统提供模块化架构来组织游戏逻辑：

```cpp
class MyLayer : public azer::Layer {
public:
    void OnAttach(azer::EngineContext& ctx) override {
        // 初始化，访问 renderer 和 window
    }
    
    void OnPhysicsUpdate(float fixedDelta) override {
        // 固定时间步物理（默认 60Hz）
    }
    
    void OnUpdate(float delta) override {
        // 每帧逻辑
    }
    
    void OnDraw() override {
        // 渲染调用
    }
    
    void OnImGuiRender() override {
        // ImGui UI
    }
};
```

**生命周期顺序**：`OnAttach` → `OnPhysicsUpdate` → `OnUpdate` → `OnInterpolate` → `OnDraw` → `OnImGuiRender` → `OnDetach`

### 2. 实体组件系统（ECS）

基于 entt，ECS 提供灵活的实体管理：

```cpp
// 创建带有组件的实体
auto entity = world.CreateEntity("Player");
world.AddComponent<TransformComponent>(entity);
world.AddComponent<RenderComponent>(entity);
world.AddComponent<PhysicsComponent>(entity);

// 查询具有特定组件的实体
auto view = world.GetAllEntitiesWith<TransformComponent, RenderComponent>();
for (auto entity : view) {
    auto& transform = view.get<TransformComponent>(entity);
    auto& render = view.get<RenderComponent>(entity);
    // 处理实体
}
```

**核心组件**：
- `IDComponent` — 唯一标识符
- `NameComponent` — 实体名称
- `TransformComponent` — 位置、旋转、缩放
- `RenderComponent` — 大小、颜色、纹理、可见性
- `PhysicsComponent` — 速度、加速度、质量
- `CollisionComponent` — AABB/球体碰撞体
- `CameraComponent` — 相机属性
- `LightComponent` — 光源属性

### 3. 渲染器抽象

抽象渲染器接口，支持可切换后端：

```cpp
class Renderer {
public:
    virtual void DrawQuad(float x, float y, float w, float h) = 0;
    virtual void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst) = 0;
    virtual void DrawModel(Model& model, const glm::mat4& transform) = 0;
    // ... 更多虚方法
};
```

**后端**：
- `SDL3Renderer` — 基于 SDL_Renderer 的 2D 渲染
- `SDL3GPURenderer` — 基于 SDL_GPU 的 3D 渲染

### 4. 事件系统

类型安全的事件分发：

```cpp
class MyLayer : public azer::Layer {
    void OnEvent(azer::Event& event) override {
        azer::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<azer::KeyPressedEvent>([this](auto& e) {
            // 处理按键
            return true;
        });
    }
};
```

### 5. 动画系统

数据驱动的关键帧动画：

```cpp
// 从 GLTF 加载动画
auto animation = Animation::LoadFromGLTF("model.gltf");

// 创建播放器
AnimationPlayer player;
player.Play(animation);
player.SetLoop(true);
player.SetSpeed(1.5f);

// 在更新循环中
player.Update(deltaTime);
```

## 性能考虑

### 内存管理

- **智能指针**：使用 `Ref<T>`（shared_ptr）共享所有权，`Scope<T>`（unique_ptr）独占所有权
- **ECS 存储**：组件存储在连续内存池中，提高缓存效率
- **预编译头**：使用 `azpch.h` 加速编译

### 渲染优化

- **批处理渲染**：分组相似绘制调用以减少状态切换
- **帧缓冲复用**：渲染到纹理用于后处理效果
- **纹理图集**：将小纹理合并为图集（手动）

### 物理优化

- **固定时间步**：60Hz 确定性物理（可配置）
- **空间分区**：对大量实体实现四叉树/八叉树（推荐）
- **组件过滤**：ECS 查询只处理具有所需组件的实体

### ECS 性能

- **缓存友好迭代**：entt 连续存储组件
- **系统排序**：按依赖顺序执行系统
- **实体回收**：entt 重用实体 ID 防止碎片化

## 示例用法

### 基础应用

```cpp
#include "Azer.h"

class MyApp : public azer::Application {
public:
    MyApp() : Application("path/to/root", azer::AppMode::Simple2D, "我的游戏") {
        PushLayer(new GameLayer());
    }
};

azer::Application* azer::CreateApplication() {
    return new MyApp();
}
```

### ECS 示例

```cpp
#include "Azer.h"

class GameLayer : public azer::Layer {
    azer::ECSScene m_Scene;
    azer::Renderer* m_Renderer = nullptr;

public:
    void OnAttach(azer::EngineContext& ctx) override {
        m_Renderer = &ctx.renderer;
        
        // 创建玩家实体
        auto player = m_Scene.CreateEntity("Player");
        auto& transform = m_Scene.GetWorld().GetComponent<azer::TransformComponent>(player);
        transform.Transform.Position = glm::vec3(100.0f, 100.0f, 0.0f);
        
        auto& render = m_Scene.GetWorld().GetComponent<azer::RenderComponent>(player);
        render.Size = glm::vec3(50.0f, 50.0f, 0.0f);
        render.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    void OnDraw() override {
        // 渲染所有实体
        auto view = m_Scene.GetWorld().GetAllEntitiesWith<azer::TransformComponent, azer::RenderComponent>();
        for (auto entity : view) {
            auto& transform = view.get<azer::TransformComponent>(entity);
            auto& render = view.get<azer::RenderComponent>(entity);
            
            if (render.Visible) {
                m_Renderer->DrawColorQuad(
                    transform.Transform.Position.x,
                    transform.Transform.Position.y,
                    render.Size.x, render.Size.y,
                    render.Color
                );
            }
        }
    }
};
```

### 自定义系统

```cpp
class MovementSystem : public azer::System {
public:
    void OnUpdate(azer::World& world, float delta) override {
        auto view = world.GetAllEntitiesWith<azer::TransformComponent, azer::PhysicsComponent>();
        
        for (auto entity : view) {
            auto& transform = view.get<azer::TransformComponent>(entity);
            auto& physics = view.get<azer::PhysicsComponent>(entity);
            
            // 根据速度更新位置
            transform.Transform.Position += physics.Velocity * delta;
            
            // 应用加速度
            physics.Velocity += physics.Acceleration * delta;
        }
    }
    
    const char* GetName() const override { return "MovementSystem"; }
};

// 注册系统
ecsLayer.GetSystemManager().RegisterSystem<MovementSystem>();
```

## 依赖

所有依赖位于 `vendor/` 目录下：

| 库 | 状态 | 用途 |
|---|------|------|
| [SDL3](https://github.com/libsdl-org/SDL) | git 子模块 | 窗口、输入、渲染 |
| [GLM](https://github.com/g-truc/glm) | git 子模块 | 数学（向量、矩阵） |
| [spdlog](https://github.com/gabime/spdlog) | git 子模块 | 日志 |
| [Dear ImGui](https://github.com/ocornut/imgui) | 直接提交 | UI（编辑器、调试） |
| [entt](https://github.com/skypjack/entt) | git clone | 实体组件系统 |
| [cgltf](https://github.com/jkuhlmann/cgltf) | 内置 | glTF 模型加载 |
| [stb](https://github.com/nothings/stb) | 内置 | 图像加载 |
| [nlohmann_json](https://github.com/nlohmann/json) | 内置 | JSON 序列化 |

## 要求

- **CMake** 3.24+
- **C++23** 编译器（GCC 13+、Clang 16+、MSVC 2022+）
- **Git**（用于子模块）

## 构建

```bash
git clone --recurse-submodules https://github.com/Trallkong/Azer-Core.git
cd Azer-Core
cd Azer && git submodule update --init --recursive && cd ..
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。