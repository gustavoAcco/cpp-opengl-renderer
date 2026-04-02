# Quickstart: Real-Time 3D OpenGL Renderer

**Branch**: `001-opengl-3d-renderer` | **Date**: 2026-04-01

---

## Prerequisites

| Requirement | Minimum version | Notes |
|-------------|-----------------|-------|
| C++ compiler | GCC 11 or Clang 13 | Must support C++20 |
| CMake | 3.20 | `cmake --version` to check |
| OpenGL driver | 3.3 Core Profile | Mesa or proprietary GPU driver |
| Python 3 | Any 3.x | Required by GLAD2 at configure time |
| Git | Any | Required by FetchContent |

All other dependencies (GLFW, GLAD, GLM, ImGui, stb_image) are fetched automatically
by CMake at configure time. No manual `apt install` or package manager commands needed.

---

## Build

```bash
# 1. Clone the repository
git clone <repo-url> cpp-opengl-renderer
cd cpp-opengl-renderer

# 2. Configure (downloads all dependencies automatically)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 3. Build
cmake --build build --parallel

# 4. Run
./build/cpp-opengl-renderer
```

For a release build (no `GL_CHECK` overhead):
```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
./build-release/cpp-opengl-renderer
```

The `compile_commands.json` (for IDE/clangd tooling) is emitted to the build
directory automatically (`CMAKE_EXPORT_COMPILE_COMMANDS ON`).

---

## Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move camera forward / left / backward / right |
| Mouse move | Rotate camera (look around) |
| Escape | Toggle mouse capture on/off |
| Alt+F4 / window × | Exit application |

---

## Debug Overlay

The ImGui overlay panel appears in the top-left corner of the window and shows:

- **FPS**: Current frames per second (updates every frame).
- **Camera position**: World-space X, Y, Z coordinates.
- **Wireframe**: Checkbox to toggle between solid and edge-only rendering.

Move the mouse over the overlay to interact with it — camera look-control suspends
automatically while the cursor is over the panel.

---

## Project Layout

```
src/core/       — Window (GLFW context) and Application (main loop)
src/renderer/   — GPU resource wrappers (Shader, Texture, VBO, VAO, IBO, Renderer)
src/camera/     — FPS camera (view/projection matrices, keyboard/mouse input)
res/shaders/    — GLSL vertex and fragment shaders
res/textures/   — Bundled texture assets
cmake/          — FetchContent dependency declarations
```

---

## Troubleshooting

**"OpenGL 3.3 is not supported"**: Update your GPU drivers or Mesa. Verify with
`glxinfo | grep "OpenGL version"`.

**"python3 not found" at configure time**: Install Python 3 (`sudo apt install python3`
on Debian/Ubuntu). Required by GLAD2 to generate the OpenGL loader.

**"Could not load texture / shader"**: Ensure you run the binary from the build
directory, or that `res/` was copied there by CMake (`file(COPY ...)` in CMakeLists.txt).

**Black window / no cube visible**: Run in Debug mode — `GL_CHECK` will abort with
an error message on the first OpenGL API failure, identifying the root cause.
