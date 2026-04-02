#include "Window.hpp"

#include <stdexcept>
#include <string>

Window::Window(int width, int height, std::string_view title)
    : m_width(width), m_height(height), m_title(title)
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialise GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL via GLAD");
    }

    // Store pointer to self for callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    // Set initial viewport
    glViewport(0, 0, m_width, m_height);
}

Window::~Window()
{
    if (m_window)
        glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->m_width  = width;
    self->m_height = height;
    glViewport(0, 0, width, height);
}
