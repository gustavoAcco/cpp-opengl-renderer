# Module Interface Contracts

**Branch**: `001-opengl-3d-renderer` | **Date**: 2026-04-01

This document defines the public C++ interfaces at the boundaries between the three
architectural layers (FR-010). These contracts govern how `Application` uses each
subsystem and must remain stable across implementation changes.

---

## Contract: Window

**Layer**: `core/`
**Consumer**: `Application`

```cpp
class Window {
public:
    // Create a GLFW window with an OpenGL 3.3 Core Profile context.
    // Throws std::runtime_error if GLFW or the context cannot be initialised.
    Window(int width, int height, std::string_view title);
    ~Window();

    // Returns true when the OS or user has requested the window to close.
    bool shouldClose() const;

    // Swap the front and back buffers (call once per frame after rendering).
    void swapBuffers();

    // Process all pending OS/input events (call once per frame at loop start).
    void pollEvents();

    // Current framebuffer dimensions (updated automatically on resize).
    int getWidth() const;
    int getHeight() const;

    // Underlying GLFW handle — passed to ImGui and GLFW input functions.
    GLFWwindow* getHandle() const;
};
```

**Guarantees**:
- After construction, an OpenGL 3.3 Core context is current on the calling thread.
- `getWidth()` / `getHeight()` always reflect the latest framebuffer size.
- Destruction releases the GLFW window and terminates GLFW.

---

## Contract: Renderer

**Layer**: `renderer/`
**Consumer**: `Application`

```cpp
class Renderer {
public:
    Renderer();

    // Clear the colour and depth buffers with the specified colour.
    void clear(float r, float g, float b, float a = 1.0f);

    // Bind vao, ibo, and shader; issue a single indexed draw call.
    // The shader must already have all uniforms set before this call.
    void draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader);

    // Switch between filled-polygon and edge-only rendering.
    // Takes effect for all subsequent draw calls in the same frame.
    void setWireframe(bool enabled);

    bool isWireframe() const;
};
```

**Guarantees**:
- `draw()` leaves no persistent OpenGL state mutations beyond `GL_POLYGON_MODE`.
- `setWireframe()` calls `glPolygonMode(GL_FRONT_AND_BACK, ...)` wrapped in `GL_CHECK`.

---

## Contract: Camera

**Layer**: `camera/`
**Consumer**: `Application`

```cpp
class Camera {
public:
    // Initialise with world-space position, yaw (degrees), and pitch (degrees).
    Camera(glm::vec3 position, float yaw = -90.0f, float pitch = 0.0f);

    // View matrix — pass to shader as u_view each frame.
    glm::mat4 getViewMatrix() const;

    // Perspective projection matrix — pass to shader as u_projection each frame.
    // aspectRatio = width / height of the current viewport.
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    // Apply keyboard movement. deltaTime in seconds (from glfwGetTime delta).
    void processKeyboard(CameraDirection direction, float deltaTime);

    // Apply mouse look. xOffset and yOffset are pixel deltas from GLFW cursor callback.
    // Caller must scale or suppress these before passing (see FR-007, R-007).
    void processMouse(float xOffset, float yOffset);

    glm::vec3 getPosition() const;
};
```

**Guarantees**:
- `processMouse()` always clamps pitch to [-89°, 89°] to prevent gimbal flip.
- `getViewMatrix()` and `getProjectionMatrix()` are pure read operations (no side effects).
- Both matrix getters are safe to call multiple times per frame.

---

## Contract: Shader

**Layer**: `renderer/`
**Consumer**: `Application`, `Renderer`

```cpp
class Shader {
public:
    // Load, compile, and link the vertex and fragment shaders at the given paths.
    // Throws std::runtime_error with the GLSL info log on failure.
    Shader(std::string_view vertexPath, std::string_view fragmentPath);
    ~Shader();

    // Bind this shader program for subsequent draw calls.
    void use() const;

    // Uniform setters — caller must call use() before setting uniforms.
    void setMat4(std::string_view name, const glm::mat4& value) const;
    void setInt(std::string_view name, int value) const;
    void setFloat(std::string_view name, float value) const;
};
```

**Guarantees**:
- Construction either succeeds fully or throws — no partially-linked shader escapes.
- All GL calls in this class are wrapped with `GL_CHECK` in debug builds.

---

## Contract: Texture

**Layer**: `renderer/`
**Consumer**: `Application`

```cpp
class Texture {
public:
    // Load a 2D image from path and upload to GPU.
    // Throws std::runtime_error if the file cannot be opened or decoded.
    explicit Texture(std::string_view path);
    ~Texture();

    // Bind to the given texture unit (slot 0 by default).
    void bind(unsigned int slot = 0) const;
    void unbind() const;
};
```

**Guarantees**:
- `stbi_set_flip_vertically_on_load(true)` is called before loading to match
  OpenGL's bottom-left UV origin.
- Construction either succeeds or throws — no partially-uploaded texture escapes.

---

## Inter-Module Call Sequence (per frame)

```
Application::run()
  │
  ├── window.pollEvents()
  │
  ├── [compute deltaTime]
  │
  ├── Application::onUpdate(deltaTime)
  │    ├── camera.processKeyboard(...)   ← if not WantCaptureMouse + cursor captured
  │    ├── camera.processMouse(...)      ← if not WantCaptureMouse + cursor captured
  │    └── [toggle wireframe, cursor via Escape key]
  │
  └── Application::onRender()
       ├── renderer.clear(...)
       ├── shader.use()
       ├── shader.setMat4("u_model", modelMatrix)
       ├── shader.setMat4("u_view", camera.getViewMatrix())
       ├── shader.setMat4("u_projection", camera.getProjectionMatrix(aspect))
       ├── texture.bind(0)
       ├── renderer.draw(vao, ibo, shader)
       ├── [ImGui new frame]
       ├── [ImGui overlay: FPS, camera.getPosition(), wireframe toggle]
       ├── [ImGui render]
       └── window.swapBuffers()
```
