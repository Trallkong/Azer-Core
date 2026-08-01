# Azer

一款轻量级、跨平台的 C++23 2D/3D 游戏引擎框架。基于**引擎即库**（engine-as-a-library）模式设计：`Azer` 编译为静态库，用户应用定义 `CreateApplication()` 并链接 `Azer`，即可获得窗口、渲染、ECS、动画等完整能力。底层由 SDL3 驱动。

## 特性

- **引擎即库** — 非可执行文件；用户只需定义 `Azer::CreateApplication()`，`EntryPoint.h` 提供 `main()`
- **可切换的渲染后端** — `SDL_2D`（SDL_Renderer）与 `Vulkan`，通过静态枚举 `RendererAPI::s_API` 选择（默认 `Vulkan`）
- **前端便利渲染器** — 静态类 `Renderer2D`（`DrawQuad`/`DrawColorQuad`/`DrawTexture`）与 `Renderer3D`（`DrawCube`/`DrawMesh`），内置默认 shader，无需手动绑定管线
- **分层更新架构** — 确定性游戏循环：固定时间步物理 + 每帧更新 + 物理插值平滑渲染
- **实体组件系统（ECS）** — 基于 entt，提供 `World`/`ECSScene` 与内置组件
- **依赖注入** — Layer 通过 `OnAttach(EngineContext&)` 获得 `Renderer&` 与 `Window&` 引用，不依赖全局单例访问引擎
- **Shader 资源系统** — `.azshader` 单一文件嵌入顶点/片元 GLSL 与管线配置，运行时经 `glslc` 编译并缓存 `.spv`
- **跨平台** — 通过 SDL3 支持 Windows、Linux、macOS

## 渲染后端

| 后端 | 枚举值 | 说明 |
|------|--------|------|
| SDL_Renderer | `RendererAPI::API::SDL_2D` | 2D 软件/硬件渲染 |
| Vulkan | `RendererAPI::API::Vulkan` | 3D 渲染（默认，功能最完整） |

> 早期基于 SDL_GPU 的后端（`SDL_GPU`）已移除；3D 渲染目前只走 Vulkan。

在构造 `Application` 之前设置后端：

```cpp
Azer::RendererAPI::s_API = Azer::RendererAPI::API::Vulkan;
```

## 快速开始

### 环境要求

- **CMake** 3.24+
- **C++23** 编译器（MSVC 2022+、GCC 13+、Clang 16+）
- **Vulkan SDK** — 硬性依赖：`Azer/vendor/CMakeLists.txt` 无条件调用 `find_package(Vulkan REQUIRED)`；运行期编译 shader 需要 `$VULKAN_SDK/bin/glslc`

### 构建

```bash
git clone --recurse-submodules <repo-url>
cd Azer && git submodule update --init --recursive
cd ..
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

CMake 选项：`AZER_ENABLE_DOCKING`（默认 OFF）为编辑器类应用启用 ImGui 停靠（docking）。

## 引擎用法

### 最小应用

```cpp
#include <Azer.h>

class MyApp : public Azer::Application {
public:
    MyApp() : Application("我的游戏") {   // 单参数：窗口标题
        PushLayer(new GameLayer());
    }
};

Azer::Application* Azer::CreateApplication() {
    return new MyApp();
}
```

### Layer 生命周期

每个 Layer 通过重写虚钩子接入主循环，调用顺序为：

`OnAttach(ctx)` → `OnPhysicsUpdate(fixedDt)` → `OnUpdate(dt)` → `OnInterpolate(alpha)` → `OnDraw` → `OnImGuiRender` → `OnDetach`

- `OnPhysicsUpdate` — 固定时间步（默认 60Hz，可用 `SetPhysicsHz()` 调整），确定性逻辑放这里
- `OnInterpolate` — 物理 tick 间的插值进度 `alpha ∈ [0,1]`，用于平滑渲染
- `OnEvent` — 事件**逆序**分发（后压入的 Layer 先处理，覆盖层优先）
- 自我移除请调用 `RequestRemove()`，`Application` 会在帧末安全地 `OnDetach` 并回收，勿在回调中直接操作 LayerStack

```cpp
class GameLayer : public Azer::Layer {
    Azer::Camera2D m_Camera;
    Azer::Ref<Azer::Texture> m_Texture;

public:
    void OnAttach(Azer::EngineContext& ctx) override {
        m_Texture = Azer::Texture::Create("./assets/textures/player.png");
    }

    void OnDraw() override {
        Azer::Renderer2D::SetCamera(m_Camera);

        Azer::Transform2D t;
        t.Position = { 100.0f, 100.0f };
        t.Rotation = 45.0f;
        Azer::Renderer2D::DrawTexture(m_Texture, t);
    }
};
```

### FileSystem 与资源根路径

`FileSystem` 以**静态单例**维护一个根路径（root），所有相对路径都基于它解析。

- **`Application` 构造时**自动调用 `FileSystem::Init(...)`，根路径指向 **Azer 引擎目录**（引擎默认资源所在处，如 `assets/shaders/quad2d.azshader`、`assets/textures/` 等）
- **派生应用类**应在构造函数中调用 `FileSystem::SetRootPath(...)` 改为**自己的目录**，这样应用资源（纹理、模型、场景）以应用目录为根
- 路径解析用 `FileSystem::ResolvePath("./assets/...")`；`Texture::Create`、`FileSystem::ReadText` 等所有资源接口都直接接受相对路径

```cpp
class MyApp : public Azer::Application {
public:
    MyApp() : Application("我的游戏") {
        // Application 构造已将根路径初始化为 Azer 引擎目录；
        // 这里覆盖为应用自身目录，之后相对路径基于它解析。
        Azer::FileSystem::SetRootPath("E:/Projects/GameDev/MyGame");

        PushLayer(new GameLayer());
    }
};

void GameLayer::OnAttach(Azer::EngineContext& ctx) {
    // 解析为 E:/Projects/GameDev/MyGame/assets/textures/player.png
    std::string full = Azer::FileSystem::ResolvePath("./assets/textures/player.png");
    m_Texture = Azer::Texture::Create("./assets/textures/player.png"); // 内部同样按 root 解析
}
```

> 参考 `VulkanTest/src/SandBox.cpp`：`Application` 构造函数（`Azer/src/base/Application.cpp`）硬编码了 Azer 目录作为初始根路径，沙盒应用随后用 `SetRootPath` 切换到自己的目录。两个路径都是**写死的绝对路径**，工作区迁移时需同步更新。

### 渲染

- **`Renderer`（抽象）** — 后端负责帧生命周期（`BeginFrame`/`EndFrame`）与底层绘制（`Draw`/`DrawIndexed`）。由 `Renderer::Create()` 依据 `s_API` 实例化对应后端
- **`RenderCommand`（静态门面）** — `Draw`/`DrawIndexed` 静态方法，转发给当前后端；自定义绘制与便利层都经由它提交
- **`Renderer2D` / `Renderer3D`（前端便利层）** — 静态类，内置单位网格与默认 shader（`quad2d`/`base3d`）。3D 侧 `DrawCube` 提供立方体；`DrawMesh` 接受用户自建的 `VertexBuffer`/`IndexBuffer`/`Shader`
- **纹理** — `Texture::Create(filePath)` 返回 `Ref<Texture>`，跨后端分派
- **相机** — `Camera2D`/`Camera3D` 通过 `SetTransform(Transform2D/3D)` 与 `SetAspectRatio` 等配置

### Shader 工作流（Vulkan）

自定义 shader 使用 `.azshader` 单文件格式（见 `assets/shaders/quad2d.azshader`）：

```glsl
@name my_shader

@vertex
#version 450
layout(location = 0) in vec3 a_Position;
// ... 顶点着色器

@fragment
#version 450
// ... 片元着色器

@pipeline
topology: triangle_list
cull_mode: back
front_face: ccw
depth_test: false
depth_write: false
blend: alpha
vertex_stride: 36   # 覆盖反射推导的 stride（引擎 VertexData 为 36 字节时）
```

- `@pipeline` 段声明拓扑/剔除/深度/混合，管线完全由 shader 派生，`vertex_stride` 用于覆盖引擎顶点缓冲（36 字节 `VertexData`）比 shader 输入更宽的情形
- 加载时 `Shader::Create(name)` 提取 GLSL → 调用 `glslc` → SPIR-V 反射构建描述符集布局/顶点输入/push-constant → 在 `assets/shaders/<name>/` 缓存 `.spv`（较新时跳过编译）
- **运行时需要 glslc 环境**（`$VULKAN_SDK/bin`）；编辑 `.azshader` 无需手动重编，缓存会自动失效
- Uniform 通过 `shader->SetUniform(name, data, size)` 上传，按反射得到的 uniform 块名寻址，buffer/描述符集/飞行帧由后端管理

### ECS

基于 entt，核心入口是 `World` 与 `ECSScene`：

```cpp
auto scene = Azer::ECSScene();
auto entity = scene.CreateEntity("Player");

auto& transform = scene.GetWorld().AddComponent<Azer::TransformComponent>(entity);
auto& render = scene.GetWorld().AddComponent<Azer::RenderComponent>(entity);
render.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
render.Size = glm::vec3(50.0f, 50.0f, 0.0f);

// 查询实体
auto view = scene.GetWorld().GetAllEntitiesWith<Azer::TransformComponent, Azer::RenderComponent>();
for (auto e : view) {
    auto& tr = view.get<Azer::TransformComponent>(e);
    auto& rd = view.get<Azer::RenderComponent>(e);
    // 处理实体...
}
```

内置组件：`IDComponent`、`NameComponent`、`TransformComponent`、`RenderComponent`、`PhysicsComponent`、`CollisionComponent`、`AnimationComponent`、`CameraComponent`、`LightComponent`、`TagComponent`、`HierarchyComponent`。

系统通过 `SystemManager` 注册（`RenderSystem`/`PhysicsSystem` 内置），`ECSLayer` 将 ECS 世界接入 Layer 生命周期。

## 目录结构

```
Azer/
├── src/
│   ├── Azer.h                    # 伞头文件（包含全部公共 API）
│   ├── azpch.h                   # 预编译头
│   ├── CMakeLists.txt            # 定义 "Azer" 静态库
│   ├── base/                     # 核心：Application、Layer、LayerStack、ImGuiLayer、
│   │   │                         #       Logger（AZ_CORE_*/AZ_* 双日志器）、Input、
│   │   │                         #       FileSystem、Random、DeltaTime、事件、动画、反射
│   ├── renderer/                 # 抽象类型：Renderer、RendererAPI、Renderer2D/3D、
│   │   │                         #       RenderCommand、Texture、Shader、Camera(2D/3D)、Mesh、Model
│   ├── ecs/                      # ECS：World、Components、SystemManager、ECSLayer、ECSScene
│   └── backends/                 # 具体实现
│       ├── SDL3Renderer/         # SDL_Renderer 后端（2D）
│       ├── SDL3Window/           # SDL3 窗口后端
│       └── Vulkan/               # Vulkan 后端（3D）
│           ├── VulkanContextManager.h/cpp   # 全局 VulkanContext 管理
│           ├── VulkanSwapchain.h/cpp
│           ├── VulkanCommandBuffer.h/cpp
│           └── VulkanRenderer/               # 渲染器实现（shader/纹理/缓冲/描述符等）
├── vendor/                       # 第三方依赖（见下表）
└── assets/shaders/               # .azshader + 编译缓存的 .spv
```

## 依赖（`vendor/`）

| 库 | 状态 | 用途 |
|----|------|------|
| [SDL3](https://github.com/libsdl-org/SDL) | git 子模块 | 窗口、输入、渲染 |
| [GLM](https://github.com/g-truc/glm) | git 子模块 | 数学（向量、矩阵） |
| [spdlog](https://github.com/gabime/spdlog) | git 子模块 | 日志 |
| [entt](https://github.com/skypjack/entt) | git 子模块 | 实体组件系统 |
| [Dear ImGui](https://github.com/ocornut/imgui) | 直接提交 | UI（编辑器、调试） |
| [cgltf](https://github.com/jkuhlmann/cgltf) | 内置 | glTF 模型加载 |
| [stb_image](https://github.com/nothings/stb) | 内置 | 图像加载 |
| [nlohmann_json](https://github.com/nlohmann/json) | 内置 | JSON 序列化 |
| [spirv_reflect](https://github.com/KhronosGroup/SPIRV-Reflect) | 内置 | SPIR-V 反射（Vulkan 管线推导） |

> 子模块需在 `Azer/` 内执行 `git submodule update --init --recursive` 初始化。

## 约定

- 智能指针别名：`Ref<T>` → `shared_ptr`，`Scope<T>` → `unique_ptr`，`Weak<T>` → `weak_ptr`，配合 `CreateRef`/`CreateScope`
- 双日志器：`AZ_CORE_*`（引擎日志）与 `AZ_*`（客户端日志）宏
- 事件 `Handled` 为私有，经 `SetHandled()`/`IsHandled()` 访问；事件以 `std::variant` 承载（`std::visit` + `Overloaded` 分发）
- `ImGuiLayer` 由 `Application` 内部创建管理，用户代码不应自行压入
- CMake 全局定义 `_CRT_SECURE_NO_WARNINGS` 与 `GLM_ENABLE_EXPERIMENTAL`（部分 GLM `gtx/*.hpp` 头依赖后者）
- 修改 `azpch.h` 需要清理重建

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。
