# Azer

一款轻量级、跨平台的 C++23 2D/3D 游戏引擎框架，基于**引擎即库**（engine-as-a-library）模式设计：`Azer` 编译为静态库，你的应用只需定义 `Azer::CreateApplication()` 并链接 `Azer`，即可获得窗口、渲染、ECS、动画等完整能力。底层由 SDL3 驱动。

## 特性

- **引擎即库** — 非可执行文件；用户只需定义 `Azer::CreateApplication()`，`EntryPoint.h` 提供 `main()`
- **可切换渲染后端** — `SDL_2D`（SDL_Renderer）与 `Vulkan`，通过静态枚举 `RendererAPI::s_API` 选择（默认 `Vulkan`）
- **前端便利渲染器** — `Renderer2D` / `Renderer3D` 静态类，内置默认 shader，无需手动绑定管线
- **分层更新架构** — 确定性游戏循环：固定时间步物理 + 每帧更新 + 物理插值平滑渲染
- **实体组件系统（ECS）** — 基于 entt，提供 `World` / `ECSScene` 与内置组件
- **依赖注入** — Layer 通过 `OnAttach(EngineContext&)` 获得 `Renderer&` 与 `Window&` 引用
- **Shader 资源系统** — `.azshader` 单文件嵌入顶点/片元 GLSL 与管线配置，运行时经 `glslc` 编译并缓存 `.spv`
- **跨平台** — 通过 SDL3 支持 Windows、Linux、macOS

## 使用方法

项目由 `create_project.py` 脚本生成：它会在**上一级目录**（与 `Azer-Core` 同级）生成一个新的应用项目（`src/main.cpp` + `CMakeLists.txt`），并自动把该项目挂到工作区。

### 1. 准备工作

- **CMake** 3.24+
- **C++23** 编译器（MSVC 2022+、GCC 13+、Clang 16+）
- **Vulkan SDK** — 硬性依赖：运行时编译 shader 需要 `$VULKAN_SDK/bin/glslc`

将 `create_project.py` 拷贝到**上一级目录**（即到 `Azer-Core` 的父目录），使其与 `Azer-Core` 同级：

```
your_workspace/
├── Azer-Core/          # 引擎（静态库）
└── create_project.py   # 项目生成脚本
```

### 2. 命令行创建项目

在父目录打开命令行，运行（`<项目名>` 必须为合法的 C++ 标识符，如 `MyGame`）：

```bash
python create_project.py MyGame
```

脚本会：
- 创建 `MyGame/src/main.cpp`（含 `Azer::CreateApplication()` 的最小应用）
- 创建 `MyGame/CMakeLists.txt`（自动链接 `Azer` 并配置包含目录）
- 在当前目录的 `CMakeLists.txt` 中添加 `add_subdirectory(MyGame)`

### 3. 编译运行

在工作区目录（父目录）执行：

```bash
cmake -B build
cmake --build build
```

生成的可执行文件名为项目名（`MyGame`）。

### 4. Visual Studio

用 Visual Studio 打开工作区的 `CMakeLists.txt`（或对应的 CMake 工程）。**选中生成后包含 `main.cpp` 的项目（`MyGame`）作为启动/生成目标**，即可编译并运行（F5）。Visual Studio 的 CMake 集成会自动识别该项目及其 `main.cpp` 入口。

> 提示：若 Visual Studio 的启动项列表中未出现 `MyGame`，请先重新生成 CMake 缓存（顶部“配置”处），或执行一次 CMake 刷新。

### 手动扩展

生成的应用是一个空壳，可在 `src/main.cpp` 中自定义：

```cpp
#include "EntryPoint.h"
#include "Azer.h"

class MyGame : public Azer::Application {
public:
    MyGame() : Application("我的游戏") {
        PushLayer(new GameLayer());
    }
};

Azer::Application* Azer::CreateApplication() {
    return new MyGame();
}
```

应用构造函数中可用 `Azer::FileSystem::SetRootPath(...)` 将资源根路径切到应用目录，从而以相对路径加载纹理、模型、场景。

## 渲染后端

| 后端 | 枚举值 | 说明 |
|------|--------|------|
| SDL_Renderer | `RendererAPI::API::SDL_2D` | 2D 软件/硬件渲染 |
| Vulkan | `RendererAPI::API::Vulkan` | 3D 渲染（默认，功能最完整） |

> 早期基于 SDL_GPU 的后端（`SDL_GPU`）已移除；3D 渲染目前只走 Vulkan。在构造 `Application` 之前设置后端：`Azer::RendererAPI::s_API = Azer::RendererAPI::API::Vulkan;`

## 目录结构

```
Azer-Core/
├── create_project.py        # 项目生成脚本（拷贝到上一级目录使用）
├── src/                     # 引擎源码
│   ├── Azer.h               # 伞头文件（包含全部公共 API）
│   ├── azpch.h              # 预编译头
│   ├── base/                # 核心：Application、Layer、LayerStack、ImGuiLayer、
│   │                        #       Logger、Input、FileSystem、Random、DeltaTime、事件、动画、反射
│   ├── renderer/            # 抽象类型：Renderer、RendererAPI、Renderer2D/3D、
│   │                        #       RenderCommand、Texture、Shader、Camera(2D/3D)、Mesh、Model
│   ├── ecs/                 # ECS：World、Components、SystemManager、ECSLayer、ECSScene
│   └── backends/            # 具体实现：SDL3Renderer（2D）、SDL3Window、Vulkan（3D）
├── vendor/                  # 第三方依赖（见下）
└── assets/shaders/          # .azshader + 编译缓存的 .spv
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

> 子模块需在 `Azer-Core/` 内执行 `git submodule update --init --recursive` 初始化。

## 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE)。
