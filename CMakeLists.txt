cmake_minimum_required(VERSION 3.20)

project(cpp-opengl-renderer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/dependencies.cmake)

add_executable(cpp-opengl-renderer
    src/main.cpp
    src/core/Window.cpp
    src/core/Application.cpp
    src/renderer/Shader.cpp
    src/renderer/Texture.cpp
    src/renderer/VertexBuffer.cpp
    src/renderer/IndexBuffer.cpp
    src/renderer/VertexArray.cpp
    src/renderer/Renderer.cpp
    src/camera/Camera.cpp
)

target_include_directories(cpp-opengl-renderer PRIVATE src)

target_link_libraries(cpp-opengl-renderer PRIVATE
    glfw
    glad_gl
    glm::glm
    imgui
    stb_image
)

# Copy assets to build directory so the binary can find them
file(COPY res/ DESTINATION ${CMAKE_BINARY_DIR}/res/)
