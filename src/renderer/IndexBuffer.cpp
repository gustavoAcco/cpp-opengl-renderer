#include "IndexBuffer.hpp"
#include "GLCheck.hpp"

IndexBuffer::IndexBuffer(std::span<const unsigned int> data)
    : m_count(static_cast<GLsizei>(data.size()))
{
    GL_CHECK(glGenBuffers(1, &m_id));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                          static_cast<GLsizeiptr>(data.size_bytes()),
                          data.data(),
                          GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::bind() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}

void IndexBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
