#include "Application.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

// ── Cube geometry ─────────────────────────────────────────────────────────────
// 24 vertices: 4 per face × 6 faces.  Layout per vertex: x,y,z,u,v (5 floats, 20 bytes).
// UVs are per-face so every face shows the full texture.
static constexpr std::array<float, 24 * 5> CUBE_VERTICES = {
    // Front (+Z)
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
    // Back (-Z)
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
    // Left (-X)
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
    // Right (+X)
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
    // Top (+Y)
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,
};

// 36 indices: 6 faces × 2 triangles × 3 vertices.  CCW winding from outside.
static constexpr std::array<unsigned int, 36> CUBE_INDICES = {
     0,  1,  2,   0,  2,  3,   // Front
     4,  5,  6,   4,  6,  7,   // Back
     8,  9, 10,   8, 10, 11,   // Left
    12, 13, 14,  12, 14, 15,   // Right
    16, 17, 18,  16, 18, 19,   // Top
    20, 21, 22,  20, 22, 23,   // Bottom
};

// ── Application ───────────────────────────────────────────────────────────────

Application::Application()
    : m_window(1280, 720, "cpp-opengl-renderer")
    , m_camera(glm::vec3(0.0f, 0.0f, 3.0f))
    , m_shader("res/shaders/basic.vert", "res/shaders/basic.frag")
    , m_vbo(CUBE_VERTICES)
    , m_ibo(CUBE_INDICES)
    , m_texture("res/textures/container.jpg")
{
    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Build VAO attribute layout (must be done while VBO is bound)
    m_vao.bind();
    m_vbo.bind();
    m_ibo.bind();
    //                index  count  type       stride  offset
    m_vao.addAttribute(0,    3,     GL_FLOAT,  20,     0);   // position (vec3)
    m_vao.addAttribute(1,    2,     GL_FLOAT,  20,     12);  // texCoord (vec2)
    m_vao.unbind();

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Capture cursor for FPS look
    glfwSetInputMode(m_window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Seed mouse position so the first delta is zero
    double mx, my;
    glfwGetCursorPos(m_window.getHandle(), &mx, &my);
    m_lastMouseX = mx;
    m_lastMouseY = my;

    m_lastTime = glfwGetTime();
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run()
{
    while (!m_window.shouldClose()) {
        m_window.pollEvents();

        double currentTime = glfwGetTime();
        auto   deltaTime   = static_cast<float>(currentTime - m_lastTime);
        m_lastTime = currentTime;

        onUpdate(deltaTime);
        onRender();
        m_window.swapBuffers();
    }
}

void Application::onUpdate(float deltaTime)
{
    GLFWwindow* win = m_window.getHandle();

    // ── Escape: toggle mouse capture (edge trigger) ──────────────────────────
    bool escNow = (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    if (escNow && !m_escWasPressed) {
        m_cursorCaptured = !m_cursorCaptured;
        glfwSetInputMode(win, GLFW_CURSOR,
                         m_cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (m_cursorCaptured) {
            // Reset last position to avoid jump when re-capturing
            double mx, my;
            glfwGetCursorPos(win, &mx, &my);
            m_lastMouseX = mx;
            m_lastMouseY = my;
            m_firstMouse = true;
        }
    }
    m_escWasPressed = escNow;

    // ── WASD camera movement ─────────────────────────────────────────────────
    if (m_cursorCaptured) {
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
            m_camera.processKeyboard(CameraDirection::FORWARD, deltaTime);
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
            m_camera.processKeyboard(CameraDirection::BACKWARD, deltaTime);
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
            m_camera.processKeyboard(CameraDirection::LEFT, deltaTime);
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
            m_camera.processKeyboard(CameraDirection::RIGHT, deltaTime);
    }

    // ── Mouse look ────────────────────────────────────────────────────────────
    if (m_cursorCaptured && !ImGui::GetIO().WantCaptureMouse) {
        double mx, my;
        glfwGetCursorPos(win, &mx, &my);

        if (m_firstMouse) {
            m_lastMouseX = mx;
            m_lastMouseY = my;
            m_firstMouse = false;
        }

        auto xOffset = static_cast<float>(mx - m_lastMouseX);
        auto yOffset = static_cast<float>(m_lastMouseY - my); // Y flipped
        m_lastMouseX = mx;
        m_lastMouseY = my;

        m_camera.processMouse(xOffset, yOffset);
    }

    // ── Cube rotation (frame-rate independent) ────────────────────────────────
    m_rotation += 45.0f * deltaTime;
}

void Application::onRender()
{
    m_renderer.clear(0.1f, 0.1f, 0.1f);

    // ── 3D scene ──────────────────────────────────────────────────────────────
    glm::mat4 model = glm::rotate(glm::mat4(1.0f),
                                  glm::radians(m_rotation),
                                  glm::vec3(0.5f, 1.0f, 0.0f));

    float aspect = static_cast<float>(m_window.getWidth()) /
                   static_cast<float>(m_window.getHeight());

    m_shader.use();
    m_shader.setMat4("u_model",      model);
    m_shader.setMat4("u_view",       m_camera.getViewMatrix());
    m_shader.setMat4("u_projection", m_camera.getProjectionMatrix(aspect));
    m_shader.setInt ("u_texture",    0);

    m_texture.bind(0);
    m_renderer.draw(m_vao, m_ibo, m_shader);

    // ── ImGui overlay ─────────────────────────────────────────────────────────
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.6f);
    ImGui::Begin("Debug", nullptr,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoMove);

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    glm::vec3 pos = m_camera.getPosition();
    ImGui::Text("Camera: %.2f  %.2f  %.2f", pos.x, pos.y, pos.z);

    if (ImGui::Checkbox("Wireframe", &m_wireframe))
        m_renderer.setWireframe(m_wireframe);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
