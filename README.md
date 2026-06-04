# Azer-Core

[English](README.md) | [中文](README_CH.md)

A lightweight, cross-platform 2D/3D game engine framework in C++23. Designed around the **engine-as-a-library** pattern: Azer builds as a static/shared library that user applications link against, with swappable rendering backends and a layered update architecture — all powered by SDL3.

## What is Azer-Core

Azer-Core is a modern C++23 game engine framework that provides:

- **Engine-as-a-library** architecture — not an executable; users define `CreateApplication()`, link `Azer`
- **Swappable renderer backends** — `Simple2D` (SDL_Renderer) and `ForwardPlus` (SDL GPU API)
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
│  │              Azer Engine Core                  │             │
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
│  │  │           Renderer Abstraction           │ │             │
│  │  │  ┌─────────────┐      ┌─────────────┐   │ │             │
│  │  │  │ SDL3Renderer│      │SDL3GPURender│   │ │             │
│  │  │  │   (2D)      │      │   (3D)      │   │ │             │
│  │  │  └─────────────┘      └─────────────┘   │ │             │
│  │  └─────────────────────────────────────────┘ │             │
│  └──────────────────────────────────────────────┘             │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────┐             │
│  │              Platform Layer                    │             │
│  │  ┌─────────────┐  ┌─────────────┐  ┌────────┐ │             │
│  │  │   SDL3      │  │   GLM       │  │ spdlog │ │             │
│  │  │  (Window/   │  │  (Math)     │  │(Log)   │ │             │
│  │  │   Input)    │  │             │  │        │ │             │
│  │  └─────────────┘  └─────────────┘  └────────┘ │             │
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
│   │   ├── Base.h                # AppMode, Ref/Scope/Weak aliases
│   │   ├── EngineContext.h       # Dependency injection context
│   │   ├── EntryPoint.h          # main() entry point
│   │   ├── Layer.h               # Layer base class
│   │   ├── LayerStack.h/cpp      # Layer container
│   │   ├── Logger.h/cpp          # spdlog-based dual logger
│   │   ├── Input.h/cpp           # Static keyboard input
│   │   ├── Window.h              # Abstract window interface
│   │   ├── GameObject.h/cpp      # Traditional entity (legacy)
│   │   ├── Scene.h/cpp           # GameObject collection
│   │   ├── SceneSerializer.h/cpp # JSON scene serialization
│   │   ├── Transform2D.h         # 2D transform
│   │   ├── Transform3D.h         # 3D transform
│   │   ├── Collision.h           # AABB/sphere collision
│   │   ├── Variant.h/cpp         # Type-erased value container
│   │   ├── event/                # Event system
│   │   ├── animation/            # Animation system
│   │   ├── reflection/           # Property reflection
│   │   └── file_system/          # File I/O
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
│   │   ├── Renderer.h/cpp        # Pure virtual renderer
│   │   ├── Camera.h              # Abstract camera
│   │   ├── Camera2D.h            # 2D camera
│   │   ├── Camera3D.h            # 3D camera
│   │   ├── Texture.h/cpp         # Abstract texture
│   │   ├── Framebuffer.h/cpp     # Abstract framebuffer
│   │   ├── Model.h/cpp           # GLTF model loader
│   │   ├── Mesh.h                # Vertex/mesh data
│   │   └── Material.h            # PBR material
│   └── backends/                 # Concrete implementations
│       ├── SDL3Renderer/         # SDL_Renderer backend
│       ├── SDL3GPURenderer/      # SDL_GPU backend
│       └── SDL3Window/           # SDL3 window backend
├── vendor/                       # Third-party dependencies
│   ├── SDL/                      # SDL3 (git submodule)
│   ├── glm/                      # GLM math (git submodule)
│   ├── spdlog/                   # spdlog logging (git submodule)
│   ├── imgui/                    # Dear ImGui (directly committed)
│   ├── entt/                     # entt ECS (git clone)
│   ├── cgltf/                    # glTF loader (vendored)
│   ├── stb/                      # stb_image (vendored)
│   └── nlohmann_json/            # JSON library (vendored)
└── assets/                       # Engine assets
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
│  │  │  ┌─────────────┐  ┌─────────────┐        │    │         │
│  │  │  │SDL3Renderer │  │SDL3GPURender│        │    │         │
│  │  │  └─────────────┘  └─────────────┘        │    │         │
│  │  └─────────────────────────────────────────┘    │         │
│  └──────────────────────────────────────────────────┘         │
│                            │                                    │
│  ┌─────────────────────────▼─────────────────────────┐         │
│  │              vendor/ (Dependencies)                │         │
│  │  ┌─────┐ ┌─────┐ ┌───────┐ ┌─────┐ ┌─────┐      │         │
│  │  │ SDL │ │ GLM │ │spdlog │ │ImGui│ │entt │      │         │
│  │  └─────┘ └─────┘ └───────┘ └─────┘ └─────┘      │         │
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
│  │  │    └─▶ BeginFrame()                                 │   ││
│  │  │    └─▶ OnDraw() for each layer                      │   ││
│  │  │    └─▶ ECS RenderSystem                             │   ││
│  │  │    └─▶ OnImGuiRender() for ImGui layers             │   ││
│  │  │    └─▶ EndFrame()                                   │   ││
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
class MyLayer : public azer::Layer {
public:
    void OnAttach(azer::EngineContext& ctx) override {
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

Abstract renderer interface with swappable backends:

```cpp
class Renderer {
public:
    virtual void DrawQuad(float x, float y, float w, float h) = 0;
    virtual void DrawTexture(Texture* tex, const SDL_FRect& src, const SDL_FRect& dst) = 0;
    virtual void DrawModel(Model& model, const glm::mat4& transform) = 0;
    // ... more virtual methods
};
```

**Backends**:
- `SDL3Renderer` — SDL_Renderer-based 2D rendering
- `SDL3GPURenderer` — SDL_GPU-based 3D rendering

### 4. Event System

Type-safe event dispatching:

```cpp
class MyLayer : public azer::Layer {
    void OnEvent(azer::Event& event) override {
        azer::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<azer::KeyPressedEvent>([this](auto& e) {
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

class MyApp : public azer::Application {
public:
    MyApp() : Application("path/to/root", azer::AppMode::Simple2D, "My Game") {
        PushLayer(new GameLayer());
    }
};

azer::Application* azer::CreateApplication() {
    return new MyApp();
}
```

### ECS Example

```cpp
#include "Azer.h"

class GameLayer : public azer::Layer {
    azer::ECSScene m_Scene;
    azer::Renderer* m_Renderer = nullptr;

public:
    void OnAttach(azer::EngineContext& ctx) override {
        m_Renderer = &ctx.renderer;
        
        // Create player entity
        auto player = m_Scene.CreateEntity("Player");
        auto& transform = m_Scene.GetWorld().GetComponent<azer::TransformComponent>(player);
        transform.Transform.Position = glm::vec3(100.0f, 100.0f, 0.0f);
        
        auto& render = m_Scene.GetWorld().GetComponent<azer::RenderComponent>(player);
        render.Size = glm::vec3(50.0f, 50.0f, 0.0f);
        render.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    void OnDraw() override {
        // Render all entities
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

### Custom System

```cpp
class MovementSystem : public azer::System {
public:
    void OnUpdate(azer::World& world, float delta) override {
        auto view = world.GetAllEntitiesWith<azer::TransformComponent, azer::PhysicsComponent>();
        
        for (auto entity : view) {
            auto& transform = view.get<azer::TransformComponent>(entity);
            auto& physics = view.get<azer::PhysicsComponent>(entity);
            
            // Update position based on velocity
            transform.Transform.Position += physics.Velocity * delta;
            
            // Apply acceleration
            physics.Velocity += physics.Acceleration * delta;
        }
    }
    
    const char* GetName() const override { return "MovementSystem"; }
};

// Register system
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
| [entt](https://github.com/skypjack/entt) | git clone | Entity Component System |
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