# Azer

[English](README.md) | [中文](README_CH.md)

A lightweight, cross-platform 2D/3D game engine framework in C++23. Designed around the **engine-as-a-library** pattern: Azer builds as a static/shared library that user applications link against, with swappable rendering backends (SDL_Renderer, SDL_GPU, Vulkan) and a layered update architecture — all powered by SDL3.

## What is Azer

Azer is a modern C++23 game engine framework that provides:

- **Engine-as-a-library** architecture — not an executable; users define `CreateApplication()`, link `Azer`
- **Swappable renderer backends** — `SDL_2D` (SDL_Renderer), `SDL_GPU` (SDL GPU API), and `Vulkan` — selected via `RendererAPI::s_API`
- **Entity Component System (ECS)** — powered by entt for flexible entity management
- **Layered update architecture** — deterministic game loop with fixed timestep physics
- **Dependency injection** — no global singletons; layers receive `EngineContext{ Renderer&, Window& }`
- **Cross-platform** — Windows, Linux, macOS via SDL3

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                      Application Layer                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │  User App   │  │  Editor     │  │  ECS Example│             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
│         │                │                │                     │
│         └────────────────┼────────────────┘                     │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────┐             │
│  │                 Azer Engine                    │             │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────┐ │             │
│  │  │   Layer     │  │   ECS       │  │ Event  │ │             │
│  │  │  System     │  │  System     │  │ System │ │             │
│  │  └──────┬──────┘  └──────┬──────┘  └────┬───┘ │             │
│  │         │                │              │      │             │
│  │  ┌──────▼────────────────▼──────────────▼───┐ │             │
│  │  │           Engine Context                  │ │             │
│  │  │        { Renderer&, Window& }             │ │             │
│  │  └───────────────────┬──────────────────────┘ │             │
│  │                      │                        │             │
│  │  ┌───────────────────▼──────────────────────┐ │             │
│  │  │           Renderer Abstraction            │ │             │
│  │  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ │ │             │
│  │  │  │SDL3Render│ │SDL3GPU   │ │ Vulkan   │ │ │             │
│  │  │  │  (2D)    │ │Render(3D)│ │Render(3D)│ │ │             │
│  │  │  └──────────┘ └──────────┘ └──────────┘ │ │             │
│  │  └─────────────────────────────────────────┘ │             │
│  └──────────────────────────────────────────────┘             │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────┐             │
│  │              Platform Layer                    │             │
│  │  ┌──────────┐ ┌──────────┐ ┌────────┐ ┌─────┐│             │
│  │  │  SDL3    │ │   GLM    │ │ spdlog │ │entt ││             │
│  │  │(Window/  │ │  (Math)  │ │ (Log)  │ │(ECS)││             │
│  │  │ Input)   │ │          │ │        │ │     ││             │
│  │  └──────────┘ └──────────┘ └────────┘ └─────┘│             │
│  └───────────────────────────────────────────────┘             │
└─────────────────────────────────────────────────────────────────┘
```

### Core Systems Interaction

```
┌─────────────────────────────────────────────────────────────────┐
│                    Game Loop (Application::Run)                  │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   Event     │    │   Physics   │    │   Render    │         │
│  │  Dispatch   │───▶│   Update    │───▶│   Frame     │         │
│  │ (Reverse)   │    │ (Fixed DT)  │    │             │         │
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
│  │                    ECS World                                ││
│  │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     ││
│  │  │  Entities   │    │ Components  │    │   Systems   │     ││
│  │  │             │    │             │    │             │     ││
│  │  └─────────────┘    └─────────────┘    └─────────────┘     ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Module Design

### Directory Structure

```
Azer/
├── src/
│   ├── Azer.h                    # Umbrella header
│   ├── azpch.h                   # Precompiled header
│   ├── base/                     # Core engine systems
│   │   ├── Application.h/cpp     # Main app loop, layer management
│   │   ├── Base.h                # Ref/Scope/Weak aliases
│   │   ├── DeltaTime.h/cpp       # Frame timing (chrono-based)
│   │   ├── EngineContext.h       # Dependency injection context
│   │   ├── EntryPoint.h          # main() entry point
│   │   ├── Layer.h               # Layer base class
│   │   ├── LayerStack.h/cpp      # Layer container
│   │   ├── ImGuiLayer.h/cpp      # Internal ImGui lifecycle
│   │   ├── Logger.h/cpp          # spdlog-based dual logger
│   │   ├── Input.h/cpp           # Keyboard input (singleton)
│   │   ├── Random.h              # Random number utility
│   │   ├── ConsoleSink.h         # In-memory log ring buffer
│   │   ├── Window.h/cpp          # Abstract window interface
│   │   ├── GameObject.h/cpp      # Traditional entity (legacy)
│   │   ├── Scene.h/cpp           # GameObject collection
│   │   ├── SceneSerializer.h/cpp # JSON scene serialization
│   │   ├── SplashLayer.h/cpp     # Optional splash screen layer
│   │   ├── Transform2D.h         # 2D transform
│   │   ├── Transform3D.h         # 3D transform
│   │   ├── Collision.h           # AABB/sphere collision
│   │   ├── Variant.h/cpp         # Type-erased value container
│   │   ├── event/                # Event system
│   │   ├── animation/            # Animation system
│   │   ├── reflection/           # Property reflection
│   │   └── file_system/          # File I/O (root-relative paths)
│   ├── ecs/                      # Entity Component System
│   │   ├── Components.h          # Component definitions
│   │   ├── World.h/cpp           # Entity/component manager
│   │   ├── System.h              # System base class
│   │   ├── SystemManager.h/cpp   # System orchestration
│   │   ├── ECSLayer.h/cpp        # ECS integration layer
│   │   ├── RenderSystem.h/cpp    # Rendering system
│   │   ├── PhysicsSystem.h/cpp   # Physics system
│   │   ├── ECSScene.h/cpp        # ECS scene management
│   │   ├── ECSSceneSerializer.h/cpp # ECS serialization
│   │   └── GameObjectWrapper.h/cpp  # Legacy bridge
│   ├── renderer/                 # Abstract renderer types
│   │   ├── Renderer.h/cpp        # Pure virtual renderer + factory
│   │   ├── RendererAPI.h/cpp     # Backend selection enum (SDL_2D/SDL_GPU/Vulkan)
│   │   ├── Camera.h              # Abstract camera
│   │   ├── Camera2D.h            # 2D camera
│   │   ├── Camera3D.h            # 3D camera
│   │   ├── Texture.h/cpp         # Abstract texture
│   │   ├── Framebuffer.h/cpp     # Abstract framebuffer
│   │   ├── Model.h/cpp           # GLTF model loader
│   │   ├── Mesh.h                # Vertex/mesh data
│   │   ├── Material.h            # PBR material
│   │   └── StbImage.cpp          # Image loading (stb_image wrapper)
│   └── backends/                 # Concrete implementations
│       ├── SDL3Renderer/         # SDL_Renderer backend (2D)
│       ├── SDL3GPURenderer/      # SDL_GPU backend (3D)
│       ├── SDL3Window/           # SDL3 window backend
│       └── Vulkan/               # Vulkan backend (3D)
│           ├── VulkanRenderer.h/cpp
│           ├── VulkanRendererContext.h/cpp
│           ├── VulkanFrameBuffer.h/cpp
│           ├── VulkanCommandBuffer.h/cpp
│           ├── VulkanGraphicPipeline.h/cpp
│           ├── VulkanShader.h/cpp
│           └── vk_mem_alloc.h/cpp
├── vendor/                       # Third-party dependencies
│   ├── SDL/                      # SDL3 (git submodule)
│   ├── glm/                      # GLM math (git submodule)
│   ├── spdlog/                   # spdlog logging (git submodule)
│   ├── imgui/                    # Dear ImGui (directly committed)
│   ├── entt/                     # entt ECS (cloned)
│   ├── cgltf/                    # glTF loader (vendored)
│   ├── stb/                      # stb_image (vendored)
│   └── nlohmann_json/            # JSON library (vendored)
└── assets/
    └── shaders/                  # GLSL shaders + SPIR-V
```

### Module Dependencies

```
┌─────────────────────────────────────────────────────────────────┐
│                    Dependency Graph                             │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   User App  │    │   Editor    │    │ ECS Example │         │
│  └──────┬──────┘    └──────┬──────┘    └──────┬──────┘         │
│         │                  │                  │                 │
│         └──────────────────┼──────────────────┘                 │
│                            │                                    │
│  ┌─────────────────────────▼─────────────────────────┐         │
│  │              Azer Engine Library                   │         │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────────┐ │         │
│  │  │   base/     │  │   ecs/      │  │ renderer/  │ │         │
│  │  │  (Core)     │  │  (ECS)      │  │(Abstract)  │ │         │
│  │  └──────┬──────┘  └──────┬──────┘  └─────┬──────┘ │         │
│  │         │                │               │        │         │
│  │         └────────────────┼───────────────┘        │         │
│  │                          │                        │         │
│  │  ┌───────────────────────▼──────────────────┐    │         │
│  │  │           backends/ (Implementations)     │    │         │
│  │  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ │    │         │
│  │  │  │SDL3Render│ │SDL3GPU   │ │ Vulkan   │ │    │         │
│  │  │  │          │ │Render    │ │ Render   │ │    │         │
│  │  │  └──────────┘ └──────────┘ └──────────┘ │    │         │
│  │  └─────────────────────────────────────────┘    │         │
│  └──────────────────────────────────────────────────┘         │
│                            │                                    │
│  ┌─────────────────────────▼─────────────────────────┐         │
│  │              vendor/ (Dependencies)                │         │
│  │  ┌──────┐ ┌──────┐ ┌───────┐ ┌──────┐ ┌──────┐  │         │
│  │  │ SDL3 │ │ GLM  │ │spdlog │ │ImGui │ │ entt │  │         │
│  │  └──────┘ └──────┘ └───────┘ └──────┘ └──────┘  │         │
│  └──────────────────────────────────────────────────┘         │
└─────────────────────────────────────────────────────────────────┘
```

## Data Flow

### Application Lifecycle

```
┌─────────────────────────────────────────────────────────────────┐
│                Application Lifecycle                            │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   Create    │    │   Init      │    │    Run      │         │
│  │Application()│───▶│  Systems    │───▶│   Game Loop │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                                                   │             │
│                                                   ▼             │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    Game Loop                                ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 1. Poll Events (SDL_PollEvent)                      │   ││
│  │  │    └─▶ Convert to typed events                      │   ││
│  │  │    └─▶ Dispatch to layers (reverse order)           │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 2. Fixed Timestep Physics (accumulator)             │   ││
│  │  │    └─▶ OnPhysicsUpdate(fixedDt) for each layer      │   ││
│  │  │    └─▶ ECS PhysicsSystem update                     │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 3. Frame Update                                     │   ││
│  │  │    └─▶ OnUpdate(dt) for each layer                  │   ││
│  │  │    └─▶ ECS systems update                           │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 4. Interpolation                                    │   ││
│  │  │    └─▶ OnInterpolate(alpha) for smooth rendering    │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 5. Render                                           │   ││
│  │  │    └─▶ ImGui::NewFrame (via internal ImGuiLayer)    │   ││
│  │  │    └─▶ OnImGuiRender() for each layer               │   ││
│  │  │    └─▶ ImGui::Render                                │   ││
│  │  │    └─▶ BeginFrame()                                 │   ││
│  │  │    └─▶ OnDraw() for each layer                      │   ││
│  │  │    └─▶ ECS RenderSystem                             │   ││
│  │  │    └─▶ EndFrame()                                   │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  │  ┌─────────────────────────────────────────────────────┐   ││
│  │  │ 6. Garbage Collection                               │   ││
│  │  │    └─▶ Remove pending layers (RequestRemove)        │   ││
│  │  └─────────────────────────────────────────────────────┘   ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### ECS Data Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    ECS Data Flow                                │
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │  Entities   │    │ Components  │    │   Systems   │         │
│  │  (IDs)      │───▶│  (Data)     │───▶│ (Behavior)  │         │
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
│  │                System Execution Order                       ││
│  │  1. PhysicsSystem (OnPhysicsUpdate)                         ││
│  │  2. AnimationSystem (OnUpdate)                              ││
│  │  3. CollisionSystem (OnUpdate)                              ││
│  │  4. RenderSystem (OnRender)                                 ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Key Systems

### 1. Layer System

The layer system provides a modular architecture for organizing game logic:

```cpp
class MyLayer : public Azer::Layer {
public:
    void OnAttach(Azer::EngineContext& ctx) override {
        // Initialize with renderer and window access
    }
    
    void OnPhysicsUpdate(float fixedDelta) override {
        // Fixed timestep physics (60Hz default)
    }
    
    void OnUpdate(float delta) override {
        // Per-frame logic
    }
    
    void OnDraw() override {
        // Render calls
    }
    
    void OnImGuiRender() override {
        // ImGui UI
    }
};
```

**Lifecycle Order**: `OnAttach` → `OnPhysicsUpdate` → `OnUpdate` → `OnInterpolate` → `OnDraw` → `OnImGuiRender` → `OnDetach`

### 2. Entity Component System (ECS)

Powered by entt, the ECS provides flexible entity management:

```cpp
// Create entity with components
auto entity = world.CreateEntity("Player");
world.AddComponent<TransformComponent>(entity);
world.AddComponent<RenderComponent>(entity);
world.AddComponent<PhysicsComponent>(entity);

// Query entities with specific components
auto view = world.GetAllEntitiesWith<TransformComponent, RenderComponent>();
for (auto entity : view) {
    auto& transform = view.get<TransformComponent>(entity);
    auto& render = view.get<RenderComponent>(entity);
    // Process entities
}
```

**Core Components**:
- `IDComponent` — Unique identifier
- `NameComponent` — Entity name
- `TransformComponent` — Position, rotation, scale
- `RenderComponent` — Size, color, texture, visibility
- `PhysicsComponent` — Velocity, acceleration, mass
- `CollisionComponent` — AABB/sphere collider
- `CameraComponent` — Camera properties
- `LightComponent` — Light properties

### 3. Renderer Abstraction

Abstract renderer interface with swappable backends, selected via `RendererAPI::s_API`:

```cpp
RendererAPI::API::SDL_2D   // SDL_Renderer (2D)
RendererAPI::API::SDL_GPU  // SDL GPU API (3D)
RendererAPI::API::Vulkan   // Vulkan (3D)
```

**Backends**:
- `SDL3Renderer` — SDL_Renderer-based 2D rendering
- `SDL3GPURenderer` — SDL_GPU-based 3D rendering
- `VulkanRenderer` — Vulkan-based 3D rendering

### 4. Event System

Type-safe event dispatching:

```cpp
class MyLayer : public Azer::Layer {
    void OnEvent(Azer::Event& event) override {
        Azer::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Azer::KeyPressedEvent>([this](auto& e) {
            // Handle key press
            return true;
        });
    }
};
```

### 5. Animation System

Data-driven animation with keyframe interpolation:

```cpp
// Load animation from GLTF
auto animation = Animation::LoadFromGLTF("model.gltf");

// Create player
AnimationPlayer player;
player.Play(animation);
player.SetLoop(true);
player.SetSpeed(1.5f);

// In update loop
player.Update(deltaTime);
```

## Performance Considerations

### Memory Management

- **Smart Pointers**: Use `Ref<T>` (shared_ptr) for shared ownership, `Scope<T>` (unique_ptr) for exclusive ownership
- **ECS Storage**: Components stored in contiguous memory pools for cache efficiency
- **Precompiled Headers**: Faster compilation with `azpch.h`

### Rendering Optimization

- **Batch Rendering**: Group similar draw calls to reduce state changes
- **Framebuffer Reuse**: Render-to-texture for post-processing effects
- **Texture Atlasing**: Combine small textures into atlases (manual)

### Physics Optimization

- **Fixed Timestep**: Deterministic physics at 60Hz (configurable)
- **Spatial Partitioning**: Implement quadtree/octtree for large entity counts (recommended)
- **Component Filtering**: ECS queries only process entities with required components

### ECS Performance

- **Cache-Friendly Iteration**: entt stores components contiguously
- **System Ordering**: Execute systems in dependency order
- **Entity Recycling**: entt reuses entity IDs to prevent fragmentation

## Example Usage

### Basic Application

```cpp
#include "Azer.h"

// Select backend before app creation
Azer::RendererAPI::s_API = Azer::RendererAPI::API::SDL_2D;

class MyApp : public Azer::Application {
public:
    MyApp() : Application("path/to/root", "My Game") {
        PushLayer(new GameLayer());
    }
};

Azer::Application* Azer::CreateApplication() {
    return new MyApp();
}
```

### ECS Example

```cpp
#include "Azer.h"

class GameLayer : public Azer::Layer {
    Azer::ECSScene m_Scene;
    Azer::Renderer* m_Renderer = nullptr;

public:
    void OnAttach(Azer::EngineContext& ctx) override {
        m_Renderer = &ctx.renderer;
        
        // Create player entity
        auto player = m_Scene.CreateEntity("Player");
        auto& transform = m_Scene.GetWorld().GetComponent<Azer::TransformComponent>(player);
        transform.Transform.Position = glm::vec3(100.0f, 100.0f, 0.0f);
        
        auto& render = m_Scene.GetWorld().GetComponent<Azer::RenderComponent>(player);
        render.Size = glm::vec3(50.0f, 50.0f, 0.0f);
        render.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    void OnDraw() override {
        auto view = m_Scene.GetWorld().GetAllEntitiesWith<Azer::TransformComponent, Azer::RenderComponent>();
        for (auto entity : view) {
            auto& transform = view.get<Azer::TransformComponent>(entity);
            auto& render = view.get<Azer::RenderComponent>(entity);
            
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

### Custom System

```cpp
class MovementSystem : public Azer::System {
public:
    void OnUpdate(Azer::World& world, float delta) override {
        auto view = world.GetAllEntitiesWith<Azer::TransformComponent, Azer::PhysicsComponent>();
        
        for (auto entity : view) {
            auto& transform = view.get<Azer::TransformComponent>(entity);
            auto& physics = view.get<Azer::PhysicsComponent>(entity);
            
            transform.Transform.Position += physics.Velocity * delta;
            physics.Velocity += physics.Acceleration * delta;
        }
    }
    
    const char* GetName() const override { return "MovementSystem"; }
};

ecsLayer.GetSystemManager().RegisterSystem<MovementSystem>();
```

## Dependencies

All under `vendor/`:

| Library | Status | Purpose |
|---------|--------|---------|
| [SDL3](https://github.com/libsdl-org/SDL) | git submodule | Window, input, rendering |
| [GLM](https://github.com/g-truc/glm) | git submodule | Math (vectors, matrices) |
| [spdlog](https://github.com/gabime/spdlog) | git submodule | Logging |
| [Dear ImGui](https://github.com/ocornut/imgui) | directly committed | UI (editor, debug) |
| [entt](https://github.com/skypjack/entt) | cloned | Entity Component System |
| [cgltf](https://github.com/jkuhlmann/cgltf) | vendored | glTF model loading |
| [stb](https://github.com/nothings/stb) | vendored | Image loading |
| [nlohmann_json](https://github.com/nlohmann/json) | vendored | JSON serialization |

## Requirements

- **CMake** 3.24+
- **C++23** compiler (GCC 13+, Clang 16+, MSVC 2022+)
- **Git** (for submodules)

## Building

```bash
git clone --recurse-submodules https://github.com/Trallkong/Azer-Core.git
cd Azer-Core
cd Azer && git submodule update --init --recursive && cd ..
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## License

MIT License — see [LICENSE](LICENSE) for details.
