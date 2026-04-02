# Implementation Plan: Real-Time 3D OpenGL Renderer

**Branch**: `001-opengl-3d-renderer` | **Date**: 2026-04-01 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-opengl-3d-renderer/spec.md`

## Summary

Build a real-time 3D OpenGL renderer in C++ that renders a textured rotating cube
in a resizable window, with FPS-style camera navigation (WASD + mouse) and a Dear
ImGui overlay displaying live FPS, camera position, and a wireframe toggle. All
external dependencies are fetched automatically at CMake configure time via
FetchContent. The codebase is organized into three clear layers: windowing/context
(core/), rendering primitives (renderer/), and camera (camera/), wired together in
Application.

## Technical Context

**Language/Version**: C++20 (GCC 11+ or Clang 13+)
**Primary Dependencies**: GLFW 3.4, GLAD2 v2.0.4 (OpenGL 3.3 Core loader), GLM 1.0.1,
Dear ImGui docking branch (pinned commit), stb_image (pinned commit from nothings/stb)
**Storage**: Asset files on disk — GLSL shaders in `res/shaders/`, textures in `res/textures/`
**Testing**: Visual output only — no testing framework
**Target Platform**: Linux, OpenGL 3.3 Core Profile driver
**Project Type**: Desktop application (real-time renderer demo)
**Performance Goals**: ≥60 FPS on OpenGL 3.3+ capable hardware; <3 s startup
**Constraints**: Single `cmake` configure + `cmake --build` invocation; C++ compiler
and OpenGL drivers are the only host prerequisites (Python 3 required for GLAD2
loader generation at configure time — pre-installed on all standard Linux distros)
**Scale/Scope**: Single window, single scene, one rotating textured cube

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Modern C++20 | ✅ PASS | C++20 standard set; `std::span` used for buffer data views |
| II. Target-Based CMake | ✅ PASS | Only `target_*` commands; cmake_minimum_required(3.20) |
| III. FetchContent | ✅ PASS | GLFW, GLAD2, GLM, ImGui, stb_image all via FetchContent with pinned versions |
| IV. Readable Code & SRP | ✅ PASS | Each class has one stated responsibility; no speculative abstractions |
| V. Header Organization | ✅ PASS | `#pragma once`; `.hpp`/`.cpp`; templates in headers |
| VI. OpenGL Error Handling | ✅ PASS | `GL_CHECK` macro wraps all GL calls in debug builds |
| VII. Shader-Only Rendering | ✅ PASS | GLSL vertex + fragment shaders; no legacy pipeline used |
| VIII. Naming Conventions | ✅ PASS | PascalCase classes, camelCase methods/vars, UPPER_SNAKE_CASE constants |

**Gate result**: All principles pass. No complexity tracking required.

*Post-design re-check*: Confirmed — no design decision violates any principle.

## Project Structure

### Documentation (this feature)

```text
specs/001-opengl-3d-renderer/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
│   └── module-interfaces.md
└── tasks.md             # Phase 2 output (/speckit.tasks command)
```

### Source Code (repository root)

```text
cpp-opengl-renderer/
├── CMakeLists.txt                    # Root build file
├── cmake/
│   └── dependencies.cmake            # All FetchContent declarations
├── src/
│   ├── main.cpp                      # Entry point; constructs Application
│   ├── core/
│   │   ├── Application.hpp / .cpp    # Main loop, owns all subsystems
│   │   └── Window.hpp / .cpp         # GLFW window + context lifecycle
│   ├── renderer/
│   │   ├── Renderer.hpp / .cpp       # Clear, draw, wireframe state
│   │   ├── Shader.hpp / .cpp         # GLSL program compile/link/bind
│   │   ├── VertexBuffer.hpp / .cpp   # GPU vertex buffer (VBO)
│   │   ├── VertexArray.hpp / .cpp    # Vertex attribute layout (VAO)
│   │   ├── IndexBuffer.hpp / .cpp    # GPU index buffer (EBO)
│   │   └── Texture.hpp / .cpp        # 2D texture load + bind
│   └── camera/
│       └── Camera.hpp / .cpp         # FPS-style view matrix, input processing
└── res/
    ├── shaders/
    │   ├── basic.vert
    │   └── basic.frag
    └── textures/
        └── container.jpg             # Default cube texture (bundled asset)
```

**Structure Decision**: Single-project layout. The three source subdirectories
(`core/`, `renderer/`, `camera/`) map directly to the three architecture layers
required by FR-010 (windowing/context, rendering primitives, and camera logic).
`main.cpp` remains minimal — it constructs and runs `Application`.

## Complexity Tracking

> No violations to justify — all constitution gates pass.
