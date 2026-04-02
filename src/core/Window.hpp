#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <string_view>

class Window {
public:
    Window(int width, int height, std::string_view title);
    ~Window();

    // Non-copyable, non-movable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    int getWidth() const  { return m_width; }
    int getHeight() const { return m_height; }
    GLFWwindow* getHandle() const { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
    int         m_width;
    int         m_height;
    std::string m_title;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
