#pragma once

#include <glad/gl.h>
#include <string_view>

class Texture {
public:
    explicit Texture(std::string_view path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(unsigned int slot = 0) const;
    void unbind() const;

private:
    GLuint m_id       = 0;
    int    m_width    = 0;
    int    m_height   = 0;
    int    m_channels = 0;
};
