# Tasks: Real-Time 3D OpenGL Renderer

**Input**: Design documents from `/specs/001-opengl-3d-renderer/`
**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, contracts/ ✅, quickstart.md ✅

**Tests**: Not requested — visual output only (no testing framework per plan.md).

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2, US3)
- File paths are relative to the repository root

---

## Phase 1: Setup (Build System)

**Purpose**: CMake project initialization and asset scaffolding — no C++ code yet.

- [X] T001 Create directory structure: `src/core/`, `src/renderer/`, `src/camera/`, `res/shaders/`, `res/textures/`, `cmake/`
- [X] T002 Write `CMakeLists.txt` — set project name, `CMAKE_CXX_STANDARD 20`, `CMAKE_EXPORT_COMPILE_COMMANDS ON`, include `cmake/dependencies.cmake`, define executable target linking GLFW, glad_gl, GLM, imgui, stb_image, add `file(COPY res/ DESTINATION ${CMAKE_BINARY_DIR}/res/)` for asset co-location (R-009)
- [X] T003 Write `cmake/dependencies.cmake` — declare all five FetchContent blocks: GLFW 3.4 (docs/tests/examples OFF), GLAD2 v2.0.4 with `glad_add_library(glad_gl STATIC … VERSION 3.3 PROFILE CORE)`, GLM 1.0.1, Dear ImGui docking branch (pinned commit) with manual `add_library(imgui STATIC …)` target, stb (pinned commit) as `INTERFACE` library (R-001, R-002, R-003, R-010)
- [X] T004 Add bundled texture asset `res/textures/container.jpg` (any royalty-free 512×512 or similar JPEG suitable for cube mapping)

**Checkpoint**: `cmake -B build -DCMAKE_BUILD_TYPE=Debug` succeeds and downloads all dependencies.

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure required before any user story rendering can start.

**⚠️ CRITICAL**: No user story work can begin until this phase is complete.

- [X] T005 Create `GL_CHECK` macro header `src/renderer/GLCheck.hpp` — `#pragma once`; in `#ifndef NDEBUG` block, define `GL_CHECK(call)` as do-while that calls `glGetError()` after `call`, prints `"OpenGL error 0x%04x at %s:%d\n"` to `stderr` and calls `std::abort()` on non-`GL_NO_ERROR`; in `#else` block, define `GL_CHECK(call) call` (R-004)
- [X] T006 [P] Implement `Window` class in `src/core/Window.hpp` and `src/core/Window.cpp` — constructor takes `(int width, int height, std::string_view title)`, initialises GLFW, requests OpenGL 3.3 Core profile, creates `GLFWwindow*`, loads GLAD via `gladLoadGL`, sets framebuffer-resize callback updating `m_width`/`m_height`, throws `std::runtime_error` on failure; implement `shouldClose()`, `swapBuffers()`, `pollEvents()`, `getWidth()`, `getHeight()`, `getHandle()`; destructor calls `glfwDestroyWindow` + `glfwTerminate` (contracts/module-interfaces.md Window contract)
- [X] T007 [P] Write `src/main.cpp` — include `core/Application.hpp`; `main()` constructs `Application` and calls `app.run()`; wrap in try/catch to print `std::runtime_error::what()` to `stderr` and return `EXIT_FAILURE` on error (FR-012)

**Checkpoint**: Project compiles to an empty or stub executable with no linker errors.

---

## Phase 3: User Story 1 — Textured Rotating Cube (Priority: P1) 🎯 MVP

**Goal**: A textured cube is visible in the window and continuously rotates without user interaction.

**Independent Test**: Launch `./build/cpp-opengl-renderer`. A window opens displaying a 3D cube with a visible texture rotating in real time.

### Implementation for User Story 1

- [X] T008 [P] [US1] Implement `VertexBuffer` in `src/renderer/VertexBuffer.hpp` and `src/renderer/VertexBuffer.cpp` — `#pragma once`; constructor takes `std::span<const float>`, calls `GL_CHECK(glGenBuffers)`, `GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, …))`, `GL_CHECK(glBufferData(…))`; `bind()` / `unbind()`; destructor calls `glDeleteBuffers`; include `GLCheck.hpp` (data-model.md VertexBuffer entity)
- [X] T009 [P] [US1] Implement `IndexBuffer` in `src/renderer/IndexBuffer.hpp` and `src/renderer/IndexBuffer.cpp` — constructor takes `std::span<const unsigned int>`, stores `m_count`; `bind()` / `unbind()` for `GL_ELEMENT_ARRAY_BUFFER`; `getCount()` returns `m_count`; destructor calls `glDeleteBuffers` (data-model.md IndexBuffer entity)
- [X] T010 [P] [US1] Implement `VertexArray` in `src/renderer/VertexArray.hpp` and `src/renderer/VertexArray.cpp` — `glGenVertexArrays` in constructor; `addAttribute(index, count, type, stride, offset)` calls `GL_CHECK(glVertexAttribPointer)` + `GL_CHECK(glEnableVertexAttribArray)`; `bind()` / `unbind()`; destructor calls `glDeleteVertexArrays` (data-model.md VertexArray entity)
- [X] T011 [P] [US1] Write GLSL shaders in `res/shaders/basic.vert` and `res/shaders/basic.frag` — vertex shader: `#version 330 core`, inputs `layout(location=0) vec3 a_position`, `layout(location=1) vec2 a_texCoord`, uniforms `mat4 u_model`, `u_view`, `u_projection`, outputs `vec2 v_texCoord`, computes `gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0)`; fragment shader: `#version 330 core`, uniform `sampler2D u_texture`, outputs `vec4 fragColor = texture(u_texture, v_texCoord)` (data-model.md Uniforms section)
- [X] T012 [P] [US1] Implement `Shader` in `src/renderer/Shader.hpp` and `src/renderer/Shader.cpp` — constructor takes `(std::string_view vertexPath, std::string_view fragmentPath)`, reads files via `std::ifstream`, compiles with `glCreateShader`/`glShaderSource`/`glCompileShader`, checks `GL_COMPILE_STATUS`, links with `glCreateProgram`/`glAttachShader`/`glLinkProgram`, checks `GL_LINK_STATUS`, throws `std::runtime_error` with info log on failure (FR-012); `use()` calls `GL_CHECK(glUseProgram)`; implement `setMat4`, `setInt`, `setFloat` via `glUniform*` wrapped in `GL_CHECK`; destructor calls `glDeleteProgram` (contracts/module-interfaces.md Shader contract)
- [X] T013 [P] [US1] Implement `Texture` in `src/renderer/Texture.hpp` and `src/renderer/Texture.cpp` — `Texture.cpp` defines `#define STB_IMAGE_IMPLEMENTATION` before `#include <stb_image.h>` (R-003); constructor takes `std::string_view path`, calls `stbi_set_flip_vertically_on_load(true)`, loads with `stbi_load`, throws `std::runtime_error` if null (FR-012), uploads with `GL_CHECK(glGenTextures)` / `glTexImage2D` / `glGenerateMipmap`, sets `GL_LINEAR_MIPMAP_LINEAR` filter; `bind(slot)` calls `GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot))` + `glBindTexture`; `unbind()`; destructor calls `glDeleteTextures` (data-model.md Texture entity)
- [X] T014 [P] [US1] Implement `Renderer` in `src/renderer/Renderer.hpp` and `src/renderer/Renderer.cpp` — `m_wireframe` bool field; `clear(r,g,b,a)` calls `GL_CHECK(glClearColor)` + `GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))`; `draw(vao, ibo, shader)` binds vao then calls `GL_CHECK(glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr))`; `setWireframe(bool)` calls `GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE or GL_FILL))`; `isWireframe()` returns `m_wireframe` (contracts/module-interfaces.md Renderer contract)
- [X] T015 [US1] Implement `Application` in `src/core/Application.hpp` and `src/core/Application.cpp` — constructor: init `m_window(1280, 720, "OpenGL Renderer")`, enable `GL_DEPTH_TEST`, define 24-vertex / 36-index cube geometry (data-model.md Cube Geometry Data section, stride=20 bytes, 5 floats per vertex), upload to `m_vbo`/`m_ibo`, configure `m_vao` with attributes (index 0: vec3 pos stride=20 offset=0; index 1: vec2 uv stride=20 offset=12), load `m_shader` from `res/shaders/basic.vert` + `basic.frag`, load `m_texture` from `res/textures/container.jpg`, set `m_lastTime = glfwGetTime()`; `run()`: loop until `m_window.shouldClose()`, compute `deltaTime = currentTime - m_lastTime`, call `onUpdate(dt)`, `onRender()`, `m_window.swapBuffers()`, `m_window.pollEvents()`; `onUpdate(dt)`: increment `m_rotation += 45.0f * deltaTime`; `onRender()`: `m_renderer.clear(0.1,0.1,0.1)`, `m_shader.use()`, set uniforms `u_model` (rotation matrix per data-model.md: `glm::rotate(I, radians(m_rotation), vec3(0.5,1.0,0.0))`), `u_view` (placeholder identity for now), `u_projection` (placeholder identity for now), `u_texture` (0), `m_texture.bind(0)`, `m_renderer.draw(m_vao, m_ibo, m_shader)`, `m_window.swapBuffers()` (note: swapBuffers called from run() not onRender())

**Checkpoint**: User Story 1 is fully functional — textured rotating cube visible on launch.

---

## Phase 4: User Story 2 — Free-Look FPS Camera (Priority: P2)

**Goal**: The user can navigate the scene freely with WASD + mouse, with Escape toggling mouse capture.

**Independent Test**: With the scene running, press WASD and move the mouse — camera moves and rotates, allowing inspection of the cube from any angle.

### Implementation for User Story 2

- [X] T016 [US2] Implement `Camera` class in `src/camera/Camera.hpp` and `src/camera/Camera.cpp` — fields: `m_position`, `m_front`, `m_up`, `m_right`, `m_yaw`, `m_pitch`, `m_movementSpeed` (2.5f), `m_mouseSensitivity` (0.1f), `m_fov` (45.0f); define `enum class CameraDirection { FORWARD, BACKWARD, LEFT, RIGHT }` in `Camera.hpp`; constructor takes `(glm::vec3 position, float yaw = -90.0f, float pitch = 0.0f)`, initialises vectors and calls `updateVectors()`; private `updateVectors()` recomputes `m_front` from yaw/pitch, then `m_right = normalize(cross(m_front, worldUp))`; `getViewMatrix()` returns `glm::lookAt(m_position, m_position + m_front, m_up)`; `getProjectionMatrix(float aspectRatio)` returns `glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f)`; `processKeyboard(CameraDirection, float deltaTime)` translates `m_position` along `m_front`/`m_right` scaled by `m_movementSpeed * deltaTime`; `processMouse(float xOffset, float yOffset)` scales by `m_mouseSensitivity`, adds to yaw/pitch, clamps pitch to `[-89.0f, 89.0f]`, calls `updateVectors()`; `getPosition()` returns `m_position` (contracts/module-interfaces.md Camera contract, R-005)
- [X] T017 [US2] Add keyboard and mouse input to `Application` in `src/core/Application.cpp` — add `m_camera(glm::vec3(0,0,3))` field; in constructor set `glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED)` and `m_cursorCaptured = true`, register GLFW cursor-position callback storing last X/Y into Application's static/member state; in `onUpdate(dt)`: read GLFW key state for W/A/S/D and call `m_camera.processKeyboard(…, dt)` when `m_cursorCaptured`; pass cursor delta to `m_camera.processMouse(xOffset, yOffset)` when `m_cursorCaptured`; replace identity matrices with `m_camera.getViewMatrix()` and `m_camera.getProjectionMatrix(width/height)` in `onRender()` (R-005, R-006, R-007, spec US2 acceptance scenarios 1–5)
- [X] T018 [US2] Implement Escape key mouse-capture toggle in `Application::onUpdate()` in `src/core/Application.cpp` — detect `GLFW_KEY_ESCAPE` pressed (edge trigger, not held); if `m_cursorCaptured`: call `glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL)`, set `m_cursorCaptured = false`; else: call `glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED)`, reset last-cursor position to avoid jump, set `m_cursorCaptured = true` (FR-013, spec US2 acceptance scenarios 6–8)

**Checkpoint**: User Stories 1 AND 2 are both functional — cube renders and camera navigates freely.

---

## Phase 5: User Story 3 — Debug Overlay Panel (Priority: P3)

**Goal**: An ImGui panel displays live FPS, camera XYZ position, and a wireframe toggle that immediately switches rendering mode.

**Independent Test**: With the scene running, verify the overlay panel is visible in a corner showing live FPS, XYZ coordinates that change on WASD movement, and a wireframe toggle that visually switches rendering mode.

### Implementation for User Story 3

- [X] T019 [US3] Initialize ImGui in `Application` constructor in `src/core/Application.cpp` — after Window and GLAD are ready: call `IMGUI_CHECKVERSION()`, `ImGui::CreateContext()`, `ImGuiIO& io = ImGui::GetIO()`, `ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true)`, `ImGui_ImplOpenGL3_Init("#version 330")`; add ImGui shutdown calls to destructor in reverse order: `ImGui_ImplOpenGL3_Shutdown()`, `ImGui_ImplGlfw_Shutdown()`, `ImGui::DestroyContext()` (R-002)
- [X] T020 [US3] Implement overlay panel in `Application::onRender()` in `src/core/Application.cpp` — after `renderer.draw(…)`: call `ImGui_ImplOpenGL3_NewFrame()`, `ImGui_ImplGlfw_NewFrame()`, `ImGui::NewFrame()`; `ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_Always)`; `ImGui::Begin("Debug")`; display FPS via `ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate)`; display camera position via `ImGui::Text("Camera: %.2f %.2f %.2f", pos.x, pos.y, pos.z)`; add `ImGui::Checkbox("Wireframe", &wireframeFlag)` and call `m_renderer.setWireframe(wireframeFlag)` when changed; `ImGui::End()`; `ImGui::Render()`; `ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData())` (FR-005, FR-006, spec US3 acceptance scenarios 1–4)
- [X] T021 [US3] Gate camera look-control on `ImGui::GetIO().WantCaptureMouse` in `Application::onUpdate()` in `src/core/Application.cpp` — wrap `m_camera.processMouse(…)` call in `if (!ImGui::GetIO().WantCaptureMouse && m_cursorCaptured)` check; ensures mouse hover over overlay suspends camera rotation without disrupting keyboard movement (FR-007, R-007, spec US3 acceptance scenario 5)

**Checkpoint**: All three user stories are independently functional — cube renders, camera navigates, overlay displays live data and wireframe toggle works.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Edge cases, resize handling, and final validation across all stories.

- [X] T022 [P] Implement framebuffer-resize handling in `src/core/Window.cpp` and `src/core/Application.cpp` — in `Window` constructor register `glfwSetFramebufferSizeCallback` that updates `m_width`/`m_height` and calls `GL_CHECK(glViewport(0, 0, width, height))`; in `Application::onRender()` recompute aspect ratio from `m_window.getWidth() / m_window.getHeight()` each frame so the projection matrix always reflects current dimensions (FR-011, spec Edge Cases viewport resize)
- [X] T023 [P] Verify error handling in `src/renderer/Shader.cpp` and `src/renderer/Texture.cpp` — confirm both constructors throw `std::runtime_error` with a human-readable message when their asset files are missing or fail to load; confirm `main.cpp` catch block prints `e.what()` to `stderr` and returns `EXIT_FAILURE`; manually test by temporarily renaming a shader/texture file (FR-012)
- [X] T024 Run full build from clean checkout per `quickstart.md` — execute `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build --parallel && ./build/cpp-opengl-renderer`; smoke-test all acceptance scenarios: cube visible and rotating (US1), WASD + mouse navigation (US2), Escape toggles cursor (US2), overlay FPS/position/wireframe toggle (US3), window resize keeps correct aspect ratio (Polish)

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies — start immediately
- **Foundational (Phase 2)**: Depends on Phase 1 — blocks all user stories
- **User Story Phases (3–5)**: All depend on Phase 2 completion; US2 integrates into US1 Application; US3 integrates into US1+US2 Application
- **Polish (Phase 6)**: Depends on all user stories complete

### User Story Dependencies

- **US1 (P1)**: Can start immediately after Phase 2; no dependencies on US2/US3
- **US2 (P2)**: Depends on US1 Application scaffold (T015) — Camera integrates into Application
- **US3 (P3)**: Depends on US1 Application scaffold (T015) — ImGui integrates into same Application

### Within Each User Story

- T008–T014 (US1 GPU primitives): fully parallel — different files, no inter-dependencies
- T015 (Application): depends on T008–T014 all complete
- T016 (Camera): can be written in parallel while T015 is in progress (different file)
- T017–T018 (Camera input): depends on T015 + T016
- T019 (ImGui init): depends on T015
- T020–T021 (overlay + mouse gate): depends on T019 + T016

---

## Parallel Example: User Story 1

```bash
# These 7 tasks can all run in parallel (different files, no inter-dependencies):
T008  Implement VertexBuffer in src/renderer/VertexBuffer.hpp/.cpp
T009  Implement IndexBuffer in src/renderer/IndexBuffer.hpp/.cpp
T010  Implement VertexArray in src/renderer/VertexArray.hpp/.cpp
T011  Write GLSL shaders in res/shaders/basic.vert and basic.frag
T012  Implement Shader in src/renderer/Shader.hpp/.cpp
T013  Implement Texture in src/renderer/Texture.hpp/.cpp
T014  Implement Renderer in src/renderer/Renderer.hpp/.cpp

# Then sequentially:
T015  Implement Application (depends on all of the above)
```

## Parallel Example: User Story 2

```bash
# T016 can overlap with T015 (different file):
T016  Implement Camera in src/camera/Camera.hpp/.cpp

# Then sequentially (depend on T015 + T016):
T017  Add keyboard + mouse input to Application
T018  Implement Escape toggle in Application
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (build system compiles)
2. Complete Phase 2: Foundational (Window + GL_CHECK)
3. Complete Phase 3: User Story 1 (textured rotating cube)
4. **STOP and VALIDATE**: Launch app, confirm cube visible and rotating
5. Demo if ready — this is a complete visual deliverable

### Incremental Delivery

1. Setup + Foundational → project builds
2. User Story 1 → textured rotating cube (MVP)
3. User Story 2 → add FPS camera navigation
4. User Story 3 → add ImGui overlay panel
5. Polish → resize, error handling, smoke test

---

## Notes

- No tests — visual output only (no testing framework in scope per plan.md)
- [P] tasks operate on different files with no shared state — safe to run in parallel
- [Story] label maps each task to its user story for traceability
- `GL_CHECK` macro (T005) must be in place before any renderer class is implemented
- `res/` is copied to the build directory by CMake (T002) — run the binary from the build directory
- Commit after each phase checkpoint to keep the repo in a always-runnable state
