# Changelog

## 2026-05-27 — Architecture Refactor (Issues #1–#17)

### 🚨 Critical

- **#1 Remove global singleton**: Deleted `Application::Get()`, introduced `EngineContext{ Renderer&, Window& }` dependency injection. All layers receive dependencies via `OnAttach(EngineContext&)`.
- **#2 Delete static proxies**: Removed stub classes `Renderer2D`, `Renderer3D`, `RenderSettings`. Callers use `Renderer*` directly.
- **#3 Shared texture ownership**: `Scope<Texture>` → `Ref<Texture>` (shared_ptr). `CreateTexture` returns `Ref<Texture>` so the same texture can share reference counts.

### 🟠 Moderate

- **#4 Backend self-cleanup**: Removed `dynamic_cast` cleanup branches from `Application` destructor. Each backend releases GPU/Renderer resources in its own destructor.
- **#5 Eliminate dynamic_cast**: Added virtual methods `ImGuiInit` / `ImGuiShutdown` / `ImGuiNewFrame` to `Renderer`. `ImGuiLayer` no longer depends on concrete backends.
- **#6 Safe layer self-removal**: Added `RequestRemove()` / `IsPendingRemove()` to `Layer`. `SplashLayer` uses end-of-frame batch cleanup — no callback needed.
- **#7 Fixed timestep**: `Application::Run()` now has a built-in accumulator. Added `OnPhysicsUpdate(fixedDt)` / `OnInterpolate(alpha)` to `Layer`. Step rate controlled by `SetPhysicsHz(float)`.
- **#8 Eliminate per-frame copy**: `LayerStack::GetLayers()` now returns `const std::vector<Layer*>&`.
- **#9 Eliminate dangling pointers**: `PopLayer` / `PopOverlay` return `void`. `LayerStack` added `PeekLayer` / `PeekOverlay`.
- **#10 Shared random engine**: Refactored `Random` to a static utility class. `m_RandomEngine` is now `inline static` — a single MT engine shared process-wide. `RandBetween` is `static`.

### 🟡 Minor

- **#11 DrawTexture camera offset**: `SDL3Renderer::DrawTexture` now accounts for `offsetX`/`offsetY`, matching `DrawColorQuad` behavior.
- **#12 Eliminate hardcoded resolution**: Removed `m_WindowWidth` / `m_WindowHeight` from `PongLayer`. Layers store a `Window*` and call `GetWindowSize()` in real time.
- **#13 Devirtualize**: `GameObject::Draw` no longer `virtual` (no subclasses override it).
- **#14 Encapsulate Event::Handled**: Changed to `private` with `SetHandled()` / `IsHandled()` accessors.
- **#15 Encapsulate Camera members**: `Camera2D` (X/Y/Zoom) and `Camera3D` (Fov/Position/Target/Up) all `private` with getters/setters.
- **#16 Optional SplashLayer**: Removed automatic `SplashLayer` push from `Application` constructor. The application layer decides whether to register it.
- **#17 Unify include guards**: `LEARNSDL_*` → `AZER_*` across 6 headers.
