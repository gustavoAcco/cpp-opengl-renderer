#include "Renderer.hpp"
#include "GLCheck.hpp"

void Renderer::clear(float r, float g, float b, float a)
{
    GL_CHECK(glClearColor(r, g, b, a));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader)
{
    shader.use();
    vao.bind();
    ibo.bind();
    GL_CHECK(glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr));
    vao.unbind();
    ibo.unbind();
}

void Renderer::setWireframe(bool enabled)
{
    m_wireframe = enabled;
    GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL));
}
