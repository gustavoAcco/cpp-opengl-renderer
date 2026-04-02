# cpp-opengl-renderer Development Guidelines

Auto-generated from all feature plans. Last updated: 2026-04-01

## Active Technologies

- **Language**: C++20 (GCC 11+ or Clang 13+)
- **Build**: CMake 3.20+, FetchContent for all deps (001-opengl-3d-renderer)
- **Window/Input**: GLFW 3.4
- **OpenGL loader**: GLAD2 v2.0.4 (OpenGL 3.3 Core Profile)
- **Math**: GLM 1.0.1
- **GUI overlay**: Dear ImGui (docking branch, pinned commit)
- **Texture loading**: stb_image (nothings/stb, pinned commit)

## Project Structure

```text
src/core/       — Window (GLFW context) and Application (main loop)
src/renderer/   — GPU resource wrappers: Shader, Texture, VertexBuffer,
                  VertexArray, IndexBuffer, Renderer
src/camera/     — FPS-style Camera (view/projection matrices, input)
res/shaders/    — GLSL vertex + fragment shaders (basic.vert, basic.frag)
res/textures/   — Bundled texture assets
cmake/          — dependencies.cmake (all FetchContent declarations)
specs/          — Feature specs, plans, tasks (speckit artifacts)
```

## Commands

```bash
# Configure (downloads all deps automatically)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --parallel

# Run
./build/cpp-opengl-renderer
```

## Code Style

- C++20 standard; use `concepts`, `ranges`, `std::span` where they improve clarity
- PascalCase classes, camelCase methods/variables, UPPER_SNAKE_CASE constants/macros
- `#pragma once` in all headers; `.hpp` interface / `.cpp` implementation
- Every OpenGL call MUST use `GL_CHECK(call)` macro in debug builds
- No legacy OpenGL (`glBegin`/`glEnd`, fixed-function pipeline) — shaders required
- Only target-scoped CMake commands (`target_include_directories`, etc.)
- All deps via FetchContent with pinned versions — no submodules, no package managers

## Constitution

See `.specify/memory/constitution.md` for the full governance document (v1.0.0).

## Recent Changes

- 001-opengl-3d-renderer: Initial renderer — textured rotating cube, FPS camera, ImGui overlay

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
