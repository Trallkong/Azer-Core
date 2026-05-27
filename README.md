# Azer Engine

A lightweight, cross-platform 2D/3D game engine framework in C++23. Designed around the **engine-as-a-library** pattern: Azer builds as a static/shared library that user applications link against, with swappable rendering backends and a layered update architecture — all powered by SDL3.

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

## Dependencies

All under `vendor/`:

| Library | Status |
|---------|--------|
| [SDL3](https://github.com/libsdl-org/SDL) | git submodule |
| [GLM](https://github.com/g-truc/glm) | git submodule |
| [spdlog](https://github.com/gabime/spdlog) | git submodule |
| [Dear ImGui](https://github.com/ocornut/imgui) | directly committed |

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
#include "EntryPoint.h"
#include <Azer.h>

class SandboxApp : public azer::Application {
public:
    SandboxApp() : Application(azer::AppMode::ForwardPlus, "Sandbox") {}

    // Add layers in OnAttach-style via constructor or a dedicated method
};

azer::Application* azer::CreateApplication() {
    return new SandboxApp();
}
```

Link against the `Azer` library:

```cmake
target_link_libraries(Sandbox PRIVATE Azer)
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
│   └── DeltaTime         Frame timing
├── renderer/         Renderer abstractions
│   ├── Renderer          Abstract renderer interface
│   ├── Camera            Abstract camera base
│   ├── Camera2D          2D camera (X/Y/Zoom)
│   ├── Camera3D          3D camera (Fov/Position/Target/Up)
│   └── Texture           Platform-agnostic texture (Ref<T>)
└── backends/         Concrete implementations
    ├── SDL3Window        SDL3 window backend
    ├── SDL3Renderer      Simple 2D backend (SDL_Renderer)
    └── SDL3GPURenderer   GPU backend (SDL_GPUDevice)
```

## License

MIT License — see [LICENSE](LICENSE) for details.
