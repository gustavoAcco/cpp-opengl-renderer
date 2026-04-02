# Data Model: Real-Time 3D OpenGL Renderer

**Branch**: `001-opengl-3d-renderer` | **Date**: 2026-04-01

This document defines the C++ data model — the entities, their fields, relationships,
and state transitions relevant to implementation.

---

## Entity: Window

**File**: `src/core/Window.hpp / .cpp`
**Responsibility**: Owns the GLFW window and OpenGL context lifecycle.

| Field | Type | Description |
|-------|------|-------------|
| `m_window` | `GLFWwindow*` | Opaque GLFW handle |
| `m_width` | `int` | Current framebuffer width (updated on resize) |
| `m_height` | `int` | Current framebuffer height (updated on resize) |
| `m_title` | `std::string` | Window title string |

**Key behaviours**:
- Constructor creates the GLFW window and sets up the framebuffer-resize callback.
- Destructor calls `glfwDestroyWindow` and `glfwTerminate`.
- `shouldClose()` delegates to `glfwWindowShouldClose`.
- `swapBuffers()` calls `glfwSwapBuffers`.
- `pollEvents()` calls `glfwPollEvents`.

**State transitions**:
```
UNINITIALIZED → OPEN (constructor) → CLOSING (shouldClose() == true) → DESTROYED (destructor)
```

---

## Entity: Shader

**File**: `src/renderer/Shader.hpp / .cpp`
**Responsibility**: Compiles, links, and binds a GLSL vertex+fragment shader program.

| Field | Type | Description |
|-------|------|-------------|
| `m_programID` | `GLuint` | OpenGL shader program handle |

**Key behaviours**:
- Constructor accepts vertex and fragment shader file paths; reads, compiles, and
  links them. Throws `std::runtime_error` on compile/link failure (FR-012).
- `use()` calls `GL_CHECK(glUseProgram(m_programID))`.
- Uniform setters: `setMat4(name, value)`, `setInt(name, value)`, `setFloat(name, value)`.
- Destructor calls `glDeleteProgram`.

**Validation**: Compile and link status checked via `glGetShaderiv` / `glGetProgramiv`;
info log extracted and thrown on failure.

---

## Entity: VertexBuffer

**File**: `src/renderer/VertexBuffer.hpp / .cpp`
**Responsibility**: Manages a single OpenGL VBO (GL_ARRAY_BUFFER).

| Field | Type | Description |
|-------|------|-------------|
| `m_id` | `GLuint` | OpenGL buffer object handle |

**Key behaviours**:
- Constructor accepts `std::span<const float>` of vertex data; calls `glGenBuffers`,
  `glBindBuffer`, `glBufferData`.
- `bind()` / `unbind()` calls `glBindBuffer(GL_ARRAY_BUFFER, m_id / 0)`.
- Destructor calls `glDeleteBuffers`.

---

## Entity: IndexBuffer

**File**: `src/renderer/IndexBuffer.hpp / .cpp`
**Responsibility**: Manages a single OpenGL EBO (GL_ELEMENT_ARRAY_BUFFER).

| Field | Type | Description |
|-------|------|-------------|
| `m_id` | `GLuint` | OpenGL buffer object handle |
| `m_count` | `GLsizei` | Number of indices stored |

**Key behaviours**:
- Constructor accepts `std::span<const unsigned int>` of index data.
- `getCount()` returns `m_count` for use in `glDrawElements`.
- Destructor calls `glDeleteBuffers`.

---

## Entity: VertexArray

**File**: `src/renderer/VertexArray.hpp / .cpp`
**Responsibility**: Owns a VAO and records vertex attribute layout from a bound VBO.

| Field | Type | Description |
|-------|------|-------------|
| `m_id` | `GLuint` | OpenGL VAO handle |

**Key behaviours**:
- `addAttribute(index, count, type, stride, offset)` calls `glVertexAttribPointer`
  and `glEnableVertexAttribArray` while the VAO is bound.
- `bind()` / `unbind()` delegates to `glBindVertexArray`.
- Destructor calls `glDeleteVertexArrays`.

**Attribute layout for cube** (set up in Application):
- Index 0: `vec3` position — count=3, type=GL_FLOAT, stride=20, offset=0
- Index 1: `vec2` texCoord — count=2, type=GL_FLOAT, stride=20, offset=12

---

## Entity: Texture

**File**: `src/renderer/Texture.hpp / .cpp`
**Responsibility**: Loads a 2D image from disk and manages the OpenGL texture object.

| Field | Type | Description |
|-------|------|-------------|
| `m_id` | `GLuint` | OpenGL texture object handle |
| `m_width` | `int` | Image width in pixels |
| `m_height` | `int` | Image height in pixels |
| `m_channels` | `int` | Number of colour channels loaded |

**Key behaviours**:
- Constructor accepts file path; loads via `stbi_load`. Throws `std::runtime_error`
  if file not found or load fails (FR-012).
- `bind(slot)` calls `glActiveTexture(GL_TEXTURE0 + slot)` then `glBindTexture`.
- `unbind()` binds texture 0.
- Destructor calls `glDeleteTextures`.
- `#define STB_IMAGE_IMPLEMENTATION` appears in `Texture.cpp` only (R-003).
- `stbi_set_flip_vertically_on_load(true)` called in constructor (OpenGL UV origin fix).

---

## Entity: Renderer

**File**: `src/renderer/Renderer.hpp / .cpp`
**Responsibility**: Issues draw calls and manages GPU render state.

| Field | Type | Description |
|-------|------|-------------|
| `m_wireframe` | `bool` | Current wireframe mode state |

**Key behaviours**:
- `clear(r, g, b, a)` calls `GL_CHECK(glClearColor(...))` + `GL_CHECK(glClear(...))`.
- `draw(vao, ibo, shader)` binds the VAO, IBO, and shader, then calls
  `GL_CHECK(glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr))`.
- `setWireframe(bool)` calls `GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE / GL_FILL))`.

---

## Entity: Camera

**File**: `src/camera/Camera.hpp / .cpp`
**Responsibility**: Maintains FPS-style view state; produces view and projection matrices.

| Field | Type | Description |
|-------|------|-------------|
| `m_position` | `glm::vec3` | World-space camera position |
| `m_front` | `glm::vec3` | Normalised look direction (derived from yaw/pitch) |
| `m_up` | `glm::vec3` | World up vector (0, 1, 0) |
| `m_right` | `glm::vec3` | Derived right vector (front × up) |
| `m_yaw` | `float` | Horizontal rotation in degrees |
| `m_pitch` | `float` | Vertical rotation in degrees, clamped [-89, 89] |
| `m_movementSpeed` | `float` | Units per second |
| `m_mouseSensitivity` | `float` | Degrees per pixel |
| `m_fov` | `float` | Vertical field of view in degrees |

**Key behaviours**:
- `getViewMatrix()` returns `glm::lookAt(m_position, m_position + m_front, m_up)`.
- `getProjectionMatrix(aspectRatio)` returns `glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f)`.
- `processKeyboard(direction, deltaTime)` translates `m_position` along `m_front` / `m_right`.
- `processMouse(xOffset, yOffset)` updates `m_yaw` and `m_pitch`, clamps pitch,
  then recomputes `m_front`, `m_right`.

**Movement directions** (enum in `Camera.hpp`):
```cpp
enum class CameraDirection { FORWARD, BACKWARD, LEFT, RIGHT };
```

---

## Entity: Application

**File**: `src/core/Application.hpp / .cpp`
**Responsibility**: Owns all subsystems; drives the main loop, per-frame update,
render, and GUI overlay.

| Field | Type | Description |
|-------|------|-------------|
| `m_window` | `Window` | Window and context owner |
| `m_renderer` | `Renderer` | Draw call issuer |
| `m_camera` | `Camera` | View state |
| `m_shader` | `Shader` | The single shader program |
| `m_vao` | `VertexArray` | Cube VAO |
| `m_vbo` | `VertexBuffer` | Cube VBO |
| `m_ibo` | `IndexBuffer` | Cube IBO |
| `m_texture` | `Texture` | Cube diffuse texture |
| `m_rotation` | `float` | Accumulated cube rotation angle (degrees) |
| `m_lastTime` | `double` | Previous frame timestamp (from glfwGetTime) |
| `m_cursorCaptured` | `bool` | Whether mouse is currently captured |

**Key behaviours**:
- Constructor: initializes GLFW, creates Window, initialises ImGui (GLFW + OpenGL3
  backends), uploads cube geometry, loads shader and texture.
- `run()`: main loop — compute deltaTime, `onUpdate(dt)`, `onRender()`, swap buffers.
- `onUpdate(deltaTime)`: process keyboard input, update camera, increment rotation,
  toggle wireframe/cursor via keyboard shortcuts.
- `onRender()`: clear, set uniforms (model/view/projection matrices), draw cube,
  then render ImGui overlay frame.

---

## Relationships

```
Application
 ├── owns → Window        (1:1)
 ├── owns → Renderer      (1:1)
 ├── owns → Camera        (1:1)
 ├── owns → Shader        (1:1)
 ├── owns → VertexArray   (1:1, for cube)
 ├── owns → VertexBuffer  (1:1, for cube)
 ├── owns → IndexBuffer   (1:1, for cube)
 └── owns → Texture       (1:1, cube diffuse)

Renderer
 └── uses → VertexArray, IndexBuffer, Shader (per draw call, passed by ref)

Shader
 └── reads → res/shaders/basic.vert, res/shaders/basic.frag (at construction)

Texture
 └── reads → res/textures/container.jpg (at construction)
```

---

## Cube Geometry Data

**24 vertices** (float layout per vertex: x, y, z, u, v — 5 floats = 20 bytes):

One vertex per face-corner, 4 vertices per face, 6 faces.
UV coordinates are per-face ([0,0]→[1,1] on each face).

**36 indices**: 6 faces × 2 triangles × 3 indices.
Face winding: counter-clockwise when viewed from outside the cube.

**Model matrix**: `glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0.5f, 1.0f, 0.0f))` — rotation around a tilted axis for visual interest.

---

## Uniforms (basic.vert)

| Name | Type | Value |
|------|------|-------|
| `u_model` | `mat4` | Per-frame rotation matrix |
| `u_view` | `mat4` | Camera view matrix |
| `u_projection` | `mat4` | Perspective projection matrix |
| `u_texture` | `sampler2D` | Texture unit 0 |
