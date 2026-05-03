# Azer Engine is a lightweight, cross-platform 2D/3D game engine framework written in modern C++23. Designed around the **engine-as-a-library** pattern, Azer provides a clean architecture with a layered event system, immediate-mode GUI debugging, and swappable rendering backends — all powered by SDL3.

## Features

- **Cross-platform** — Windows, Linux, macOS via SDL3
- **Swappable renderer backends**
  - `Simple2D` — SDL_Renderer-based, ideal for simple 2D apps
  - `ForwardPlus` — SDL GPU API-based, suitable for complex 2D/3D rendering
- **Layered architecture** — Layer/LayerStack system with attach/detach/update/draw/event/ImGui callbacks
- **Event system** — Typed event hierarchy with SDL event conversion and dispatch
- **Dear ImGui integration** — Built-in ImGui layer for debug UI and tools
- **Dual logger** — spdlog-powered core and client loggers (AZ_CORE_*, AZ_*)
- **Precompiled headers** — Faster compile times
- **Modern C++** — C++23, smart pointers, RAII

## Dependencies

All dependencies are managed as git submodules under `vendor/`:

| Library | Purpose |
|---------|---------|
| [SDL3](https://github.com/libsdl-org/SDL) | Windowing, input, rendering, GPU API |
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate-mode debug GUI |
| [GLM](https://github.com/g-truc/glm) | Mathematics (vectors, matrices) |
| [spdlog](https://github.com/gabime/spdlog) | Fast asynchronous logging |

## Requirements

- **CMake** 3.24+
- **C++23** compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- **Git** (for submodules)

## Building

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/Trallkong/Azer-Core.git
cd Azer-Core

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Quick Start

Create your own application by defining `CreateApplication()`:

```cpp
#include "EntryPoint.h"
#include <Azer.h>

class SandboxApp : public azer::Application {
public:
    SandboxApp() : Application(azer::AppMode::ForwardPlus, "Sandbox") {}

    void OnCreate() override {
        AZ_INFO("Sandbox created!");
    }
};

azer::Application* azer::CreateApplication() {
    return new SandboxApp();
}
```

Link against the `Azer` library in your CMake project:

```cmake
target_link_libraries(Sandbox PRIVATE Azer)
```

## Architecture

```
Azer.h (umbrella header)
├── base/           Core engine systems
│   ├── Application     Central engine singleton
│   ├── EntryPoint      Provides main() entry
│   ├── Layer           Layer base class
│   ├── LayerStack      Layer management (push/pop)
│   ├── Event           Typed event system
│   ├── ImGuiLayer      Dear ImGui integration
│   ├── SplashLayer     Engine splash screen
│   ├── Logger          spdlog dual-logger
│   └── DeltaTime       Frame timing
├── renderer/       Renderer abstractions
│   ├── Renderer         Abstract renderer interface
│   ├── Renderer2D       2D drawing facade
│   └── Texture          Platform-agnostic texture
└── backends/       Concrete renderer implementations
    ├── SDL3Renderer     Simple 2D backend (SDL_Renderer)
    └── SDL3GPURenderer  GPU backend (SDL_GPUDevice)
```

## License

This project is currently unlicensed. All rights reserved by the author.
