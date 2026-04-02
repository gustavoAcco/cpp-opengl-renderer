#pragma once

#include <glad/gl.h>
#include <span>

class VertexBuffer {
public:
    explicit VertexBuffer(std::span<const float> data);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void bind() const;
    void unbind() const;

private:
    GLuint m_id = 0;
};
