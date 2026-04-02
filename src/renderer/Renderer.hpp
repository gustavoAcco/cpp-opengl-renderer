#pragma once

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

class Renderer {
public:
    Renderer() = default;

    void clear(float r, float g, float b, float a = 1.0f);
    void draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader);
    void setWireframe(bool enabled);
    bool isWireframe() const { return m_wireframe; }

private:
    bool m_wireframe = false;
};
