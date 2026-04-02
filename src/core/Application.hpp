#pragma once

#include "Window.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/Shader.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/VertexArray.hpp"
#include "renderer/IndexBuffer.hpp"
#include "renderer/Texture.hpp"
#include "camera/Camera.hpp"

class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void onUpdate(float deltaTime);
    void onRender();

    Window       m_window;
    Renderer     m_renderer;
    Camera       m_camera;
    Shader       m_shader;
    VertexArray  m_vao;
    VertexBuffer m_vbo;
    IndexBuffer  m_ibo;
    Texture      m_texture;

    float  m_rotation      = 0.0f;
    double m_lastTime      = 0.0;
    bool   m_cursorCaptured = true;

    // Mouse tracking
    double m_lastMouseX    = 0.0;
    double m_lastMouseY    = 0.0;
    bool   m_firstMouse    = true;

    // Edge-trigger for Escape key
    bool   m_escWasPressed = false;

    // Wireframe state (shared with overlay)
    bool   m_wireframe     = false;
};
