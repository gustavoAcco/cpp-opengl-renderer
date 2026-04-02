#pragma once

#include <glad/gl.h>

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void bind() const;
    void unbind() const;

    // Call while this VAO is bound and the VBO is bound to GL_ARRAY_BUFFER.
    void addAttribute(GLuint index, GLint count, GLenum type,
                      GLsizei stride, GLsizeiptr offset);

private:
    GLuint m_id = 0;
};
