# Research: Real-Time 3D OpenGL Renderer

**Branch**: `001-opengl-3d-renderer` | **Date**: 2026-04-01
**Purpose**: Resolve all technical unknowns before design and task decomposition.

---

## R-001: GLAD2 FetchContent Integration

**Decision**: Use `Dav1dde/glad` (glad2) v2.0.4 via FetchContent with `glad_add_library()`.

**Rationale**: GLAD2 is the officially maintained OpenGL loader generator. Its v2
branch has first-class CMake support: after fetching, `glad_add_library(glad_gl
STATIC LANGUAGE C API gl VERSION 3.3 PROFILE CORE)` creates a proper CMake target
with no extra boilerplate. This avoids committing generated files to the repository
and keeps the loader pinned to an exact version.

**Python 3 requirement**: GLAD2 generates the loader source at CMake configure time
using Python 3. Python 3 is pre-installed on all standard Linux distributions (Ubuntu,
Fedora, Arch, etc.) and is not a "manual installation" step for the target audience.
This is the only deviation from pure zero-dependency configure; it is acceptable.

**Alternatives considered**:
- GLEW: rejected — user explicitly specified GLAD.
- Pre-generated GLAD files committed to repo: rejected — violates FetchContent-only rule.
- Community glad-cmake wrappers: rejected — unmaintained, unofficial.

**CMake snippet** (in `cmake/dependencies.cmake`):
```cmake
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.4
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl STATIC LANGUAGE C API gl VERSION 3.3 PROFILE CORE)
```

---

## R-002: Dear ImGui Docking Branch FetchContent

**Decision**: Fetch `ocornut/imgui` at a pinned commit on the docking branch;
create a `STATIC` CMake target manually from the required source files.

**Rationale**: The docking branch has no version tags and no CMakeLists.txt.
Fetching a pinned commit and declaring the target inline is the standard pattern
for ImGui + FetchContent. The minimal required sources are:
- `imgui.cpp`, `imgui_draw.cpp`, `imgui_tables.cpp`, `imgui_widgets.cpp`
- `backends/imgui_impl_glfw.cpp`, `backends/imgui_impl_opengl3.cpp`

The pinned commit used: `v1.91.6` merge into docking (commit `9e8df45` from the
`docking` branch, January 2025). Exact hash to be confirmed at implementation time
from the docking branch HEAD at a tagged release merge point.

**Alternatives considered**:
- Using the master branch (non-docking): rejected — user explicitly specified docking.
- Using a FetchContent wrapper repo for ImGui: rejected — unofficial, potentially stale.

**CMake snippet** (in `cmake/dependencies.cmake`):
```cmake
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        <pinned-docking-commit-hash>
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PRIVATE glfw glad_gl)
```

---

## R-003: stb_image FetchContent Integration

**Decision**: Fetch `nothings/stb` at a pinned commit as a header-only source
drop; create an `INTERFACE` library that exposes the include path, and define
`STB_IMAGE_IMPLEMENTATION` in exactly one `.cpp` file in the project.

**Rationale**: stb_image is a single-header library. The implementation must be
compiled in exactly one translation unit. The cleanest pattern: an `INTERFACE`
CMake target exposes the header path; `src/renderer/Texture.cpp` defines
`STB_IMAGE_IMPLEMENTATION` before including `stb_image.h` — this is the one
translation unit that compiles the implementation.

**CMake snippet**:
```cmake
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        <pinned-commit-hash>
)
FetchContent_MakeAvailable(stb)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
```

In `src/renderer/Texture.cpp`:
```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
```

---

## R-004: GL_CHECK Macro Design

**Decision**: Implement `GL_CHECK(call)` as a macro that executes the call, then
checks `glGetError()` in debug builds. On error, logs the error code, file, and
line number to `stderr`, then calls `std::abort()`.

**Rationale**: The constitution (Principle VI) mandates this macro for all OpenGL
calls in debug builds. Using `glGetError()` after each call (rather than
`glDebugMessageCallback`) is simpler for a demo and sufficient to catch the vast
majority of API misuse errors. `NDEBUG` guards the check so release builds have
zero overhead.

**Implementation** (in a header included by all renderer files):
```cpp
#ifndef NDEBUG
  #define GL_CHECK(call)                                            \
      do {                                                           \
          call;                                                      \
          GLenum err = glGetError();                                 \
          if (err != GL_NO_ERROR) {                                  \
              std::fprintf(stderr, "OpenGL error 0x%04x at %s:%d\n",\
                           err, __FILE__, __LINE__);                 \
              std::abort();                                          \
          }                                                          \
      } while (false)
#else
  #define GL_CHECK(call) call
#endif
```

**Alternatives considered**:
- `glDebugMessageCallback`: more powerful but requires OpenGL 4.3+ context flag;
  overkill for a 3.3 Core demo.
- Asserting without logging: rejected — logs are required for diagnosis.

---

## R-005: FPS Camera Mathematics

**Decision**: Euler-angle (yaw/pitch) FPS camera. View matrix computed each frame
via `glm::lookAt(position, position + front, worldUp)`.

**Rationale**: FPS cameras only need two degrees of freedom (no roll). Euler angles
are simpler than quaternions for this case and trivially clamped. `glm::lookAt`
handles the matrix construction correctly.

**Pitch clamping**: Clamp pitch to `[-89.0f, 89.0f]` degrees to prevent the camera
from flipping at the poles (spec acceptance scenario US2-4).

**Movement**: Translate `position` along `front` and `right` vectors each frame,
scaled by `movementSpeed * deltaTime` (delta-time from `glfwGetTime()` difference).

**Mouse sensitivity**: Apply a scalar (e.g., 0.1) to raw mouse delta before adding
to yaw/pitch. Configurable constant in `Camera.hpp`.

---

## R-006: Delta Time Calculation

**Decision**: Use `glfwGetTime()` to compute per-frame delta time.

**Rationale**: GLFW provides a monotonic timer (`glfwGetTime()` in seconds since
GLFW init). Per-frame delta = `currentTime - lastTime`. This is the standard
approach for frame-rate-independent motion in OpenGL applications.

**Implementation** (in `Application::run()` loop):
```cpp
double currentTime = glfwGetTime();
float deltaTime = static_cast<float>(currentTime - m_lastTime);
m_lastTime = currentTime;
```

---

## R-007: ImGui Mouse Passthrough (FR-007)

**Decision**: Check `ImGui::GetIO().WantCaptureMouse` each frame; suppress camera
look-control input when true.

**Rationale**: When the ImGui overlay is hovered and the user interacts with it,
ImGui sets `io.WantCaptureMouse = true`. Checking this flag before processing GLFW
mouse delta input prevents the camera from rotating while the user is clicking the
wireframe toggle.

**Implementation**: In `Application::onUpdate()`, gate mouse-delta processing:
```cpp
if (!ImGui::GetIO().WantCaptureMouse) {
    m_camera.processMouse(xOffset, yOffset);
}
```

Additionally, Escape key toggles `glfwSetInputMode(window, GLFW_CURSOR,
GLFW_CURSOR_DISABLED / GLFW_CURSOR_NORMAL)` and the corresponding `m_cursorCaptured`
flag (FR-013).

---

## R-008: Textured Cube Geometry

**Decision**: 24 unique vertices (4 per face) with an index buffer (36 indices,
6 faces × 2 triangles × 3 indices). Each vertex stores `vec3 position` + `vec2 texCoord`.

**Rationale**: Using 24 vertices (not 8) is necessary because each face needs its
own UV coordinates — a shared-vertex cube would require UV seams that can't be
expressed with a single vertex. An index buffer (36 indices) keeps GPU memory usage
minimal and is idiomatic OpenGL.

**Vertex layout** (stride = 20 bytes):
- Attribute 0: `vec3` position (offset 0)
- Attribute 1: `vec2` texCoord (offset 12)

No normals are included (no lighting in scope per spec Assumptions).

---

## R-009: Shader Asset Path Resolution

**Decision**: Derive the `res/` directory path relative to the compiled executable
using `std::filesystem`. The executable is placed in the build tree; shaders and
textures are accessed as `<exe_dir>/../res/shaders/` and `<exe_dir>/../res/textures/`
when running from the build directory.

**Alternative — CMake configure_file**: Copy shaders to the build directory at
configure time using `configure_file` or `file(COPY ...)`. This is simpler and
avoids filesystem path arithmetic at runtime.

**Selected**: Use CMake `file(COPY res/ DESTINATION ${CMAKE_BINARY_DIR}/res/)` so
that `res/` is co-located with the executable in the build output. The Shader and
Texture classes then accept absolute paths constructed by the caller in `Application`.

---

## R-010: GLM and GLFW FetchContent Pins

**GLFW**: `glfw/glfw`, tag `3.4`. Options: `GLFW_BUILD_DOCS OFF`, `GLFW_BUILD_TESTS OFF`,
`GLFW_BUILD_EXAMPLES OFF`.

**GLM**: `g-truc/glm`, tag `1.0.1`. Header-only; `glm::glm` target available
after `FetchContent_MakeAvailable`.
