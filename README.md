# Azer Engine

[English](README.md) | [中文](README_CH.md)

A lightweight, cross-platform 2D/3D game engine framework in C++23. Designed around the **engine-as-a-library** pattern: Azer builds as a static/shared library that user applications link against, with swappable rendering backends and a layered update architecture — all powered by SDL3.

## Showcase

Applications built with Azer-Core:

![3D Viewer](assets/showcase/3d_viewer_show.png)
*3D Model Viewer — built with `Azer` engine using the `ForwardPlus` GPU backend*

## Features

- **Cross-platform** — Windows, Linux, macOS via SDL3
- **Engine-as-a-library** — not an executable; users define `CreateApplication()`, link `Azer`
- **Swappable renderer backends**
  - `Simple2D` — SDL_Renderer-based, ideal for 2D apps
  - `ForwardPlus` — SDL GPU API-based, suitable for complex 2D/3D rendering
- **Layered architecture** — Layer base class with `OnAttach` → `OnPhysicsUpdate(fixedDt)` → `OnUpdate(dt)` → `OnInterpolate(alpha)` → `OnDraw` → `OnImGuiRender` → `OnDetach` lifecycle
- **Fixed timestep physics** — configurable step rate (`SetPhysicsHz`), accumulator-driven, with interpolation for smooth rendering
- **Dependency injection** — no global singletons; layers receive `EngineContext{ Renderer&, Window& }` on attach
- **Typed event system** — SDL event → typed event conversion, dispatch via `EventDispatcher`
- **Dear ImGui integration** — built-in `ImGuiLayer` with backend-agnostic init/shutdown/newframe
- **Safe layer self-removal** — layers call `RequestRemove()`, cleaned up at frame end
- **Dual logger** — spdlog-powered core and client loggers (`AZ_CORE_*`, `AZ_*`)
- **Precompiled headers** — faster compile times
- **Modern C++** — C++23, smart pointers (`Ref<T>`/`Scope<T>`), RAII

### 3D Rendering (ForwardPlus Backend)

- **GLTF Model Loading** — `Model::LoadGLTF(filepath)` loads meshes, materials, and textures from `.gltf`/`.glb` files
- **Skybox Rendering** — `DrawSkybox(hdrTexture)` renders HDR environment maps as skyboxes
- **PBR Material System** — `Material` struct with `BaseColorFactor`, `MetallicFactor`, `RoughnessFactor`, and texture indices
- **Mesh System** — `Vertex` (Position, Normal, TexCoord), `Mesh` with indexed drawing
- **3D Camera** — `Camera3D` with perspective projection, position/target/up vectors, configurable FOV

### Animation System

- **Variant Type Container** — runtime-typed value holder (`Float`, `Vec2`, `Vec3`, `Quat`) with zero-heap inline storage and `Interpolate()` support
- **Property Accessor** — type-erased property binding (`void*` + `VariantType` + optional `SetterFn`) for runtime attribute modification
- **Keyframe Animation** — `Animation` → `AnimationChannel` → `KeyFrame` data hierarchy, matching glTF animation structure
- **Animation Player** — playback controller with `Play`/`Stop`/`Pause`/`Resume`, configurable speed and looping, per-frame sampling with linear interpolation

### Scene & GameObject

- **GameObject** — entity with unique ID, name, `Transform3D`, size, color, texture, and visibility
- **Scene** — owns a collection of `Scope<GameObject>`; supports `CreateObject`, `AddObject`, `RemoveObject`, `TakeObject` (for undo), and `FindObject`
- **Transform2D / Transform3D** — position, rotation (euler angles in degrees), scale with `GetMatrix()`; `Transform3D` adds `GetForward()`/`GetRight()`/`GetUp()` direction vectors
- **Scene Serializer** — `SceneSerializer::Save`/`Load` to/from JSON files with relative asset path resolution

### Collision Detection

- **AABB2D / AABB3D** — axis-aligned bounding boxes with `Contains`, `Intersects`, `Union`, `ExpandToInclude`, `Offset`
- **Collision namespace** — `Intersects`, `PointInAABB`, `CircleVsCircle`, `CircleVsAABB` (2D); `SphereVsSphere`, `SphereVsAABB` (3D); `RayVsAABB` / `RayVsAABB2D` for 3D/2D picking

### Framebuffer

- **Framebuffer** — abstract render-to-texture target with `Resize`, color/depth texture access; concrete backends in `SDL3Framebuffer` and `SDL3GPUFramebuffer`

### Utilities

- **Input System** — `Input::IsKeyPressed(key)` for polling keyboard state
- **Random Engine** — `Random::RandBetween(min, max)` shared Mersenne Twister, seeded once
- **Texture Factory** — `Texture::Create()` / `Texture::CreateHDR()` with shared ownership via `Ref<Texture>`
- **Camera System** — `Camera2D` (ortho, X/Y/Zoom) and `Camera3D` (perspective, FOV/Position/Target/Up) with encapsulated state
- **File System** — `FileSystem` with root-path-based resolution, text/binary read/write, directory listing, and path utilities

## Dependencies

All under `vendor/`:

| Library | Status |
|---------|--------|
| [SDL3](https://github.com/libsdl-org/SDL) | git submodule |
| [GLM](https://github.com/g-truc/glm) | git submodule |
| [spdlog](https://github.com/gabime/spdlog) | git submodule |
| [Dear ImGui](https://github.com/ocornut/imgui) | directly committed |
| [cgltf](https://github.com/jkuhlmann/cgltf) | vendored |
| [stb](https://github.com/nothings/stb) | vendored |
| [nlohmann_json](https://github.com/nlohmann/json) | vendored |

## Requirements

- **CMake** 3.24+
- **C++23** compiler (GCC 13+, Clang 16+, MSVC 2022+)
- **Git** (for submodules)

## Building

```bash
git clone --recurse-submodules https://github.com/Trallkong/Azer-Core.git
cd Azer-Core
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Quick Start

```cpp
#include "Azer.h"

class MyApp : public azer::Application {
public:
    MyApp() : Application(azer::AppMode::ForwardPlus, "My 3D App") {}
};

azer::Application* azer::CreateApplication() {
    return new MyApp();
}
```

Create a layer:

```cpp
class MyLayer : public azer::Layer {
public:
    void OnAttach(azer::EngineContext& ctx) override {
        // Access renderer and window via ctx
        m_Model = azer::Model::LoadGLTF("assets/model.gltf", ctx.renderer);
    }

    void OnUpdate(float dt) override {
        // Per-frame logic
    }

    void OnDraw() override {
        // Render calls
    }

private:
    azer::Scope<azer::Model> m_Model;
};
```

Link against `Azer` and `vendor`:

```cmake
target_link_libraries(MyApp Azer vendor)
```

## Architecture

```
Azer.h (umbrella header)
├── base/             Core engine systems
│   ├── Application       Engine orchestrator (no singleton)
│   ├── EntryPoint        Provides main() entry
│   ├── EngineContext     Renderer+Window dependency injection
│   ├── Layer             Layer base class
│   ├── LayerStack        Layer management (push/pop/peek)
│   ├── event/Event       Typed event system
│   ├── ImGuiLayer        Dear ImGui integration (as a Layer)
│   ├── SplashLayer       Optional splash screen (not auto-pushed)
│   ├── Logger            spdlog dual-logger
│   ├── Window            Abstract window interface
│   ├── Input             Static key state queries
│   ├── Random            Shared Mersenne Twister utility
│   ├── DeltaTime         Frame timing
│   ├── GameObject        Entity with ID, transform, color, texture
│   ├── Scene             GameObject collection manager
│   ├── SceneSerializer   JSON scene save/load
│   ├── Collision         AABB2D/3D + ray/sphere/circle intersection tests
│   ├── Transform2D       2D transform (Position, Rotation, Scale)
│   ├── Transform3D       3D transform with euler angles + direction vectors
│   ├── Variant           Runtime-typed value container (Float/Vec2/Vec3/Quat)
│   ├── file_system/
│   │   └── FileSystem    Root-path-based file I/O + directory listing
│   ├── animation/
│   │   ├── Animation         Animation data (channels + keyframes)
│   │   └── AnimationPlayer   Playback controller (play/stop/pause/loop)
│   └── reflection/
│       └── PropertyAccessor  Type-erased property binding
├── renderer/         Renderer abstractions
│   ├── Renderer          Abstract renderer interface
│   ├── Camera            Abstract camera base
│   ├── Camera2D          2D camera (X/Y/Zoom)
│   ├── Camera3D          3D camera (Fov/Position/Target/Up)
│   ├── Texture           Platform-agnostic texture (Ref<T>)
│   ├── Framebuffer       Abstract render-to-texture target
│   ├── Model             GLTF model loader
│   ├── Mesh              Vertex/Index data structures
│   └── Material          PBR material properties
└── backends/         Concrete implementations
    ├── SDL3Window        SDL3 window backend
    ├── SDL3Renderer      Simple 2D backend (SDL_Renderer)
    └── SDL3GPURenderer   GPU backend (SDL_GPUDevice)
```

## License

MIT License — see [LICENSE](LICENSE) for details.
