#include "VertexArray.hpp"
#include "GLCheck.hpp"

VertexArray::VertexArray()
{
    GL_CHECK(glGenVertexArrays(1, &m_id));
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const
{
    GL_CHECK(glBindVertexArray(m_id));
}

void VertexArray::unbind() const
{
    GL_CHECK(glBindVertexArray(0));
}

void VertexArray::addAttribute(GLuint index, GLint count, GLenum type,
                               GLsizei stride, GLsizeiptr offset)
{
    GL_CHECK(glVertexAttribPointer(index, count, type, GL_FALSE, stride,
                                   reinterpret_cast<const void*>(offset)));
    GL_CHECK(glEnableVertexAttribArray(index));
}
