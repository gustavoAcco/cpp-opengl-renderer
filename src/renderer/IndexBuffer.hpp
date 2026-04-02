#pragma once

#include <glad/gl.h>
#include <span>

class IndexBuffer {
public:
    explicit IndexBuffer(std::span<const unsigned int> data);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void bind() const;
    void unbind() const;

    GLsizei getCount() const { return m_count; }

private:
    GLuint  m_id    = 0;
    GLsizei m_count = 0;
};
