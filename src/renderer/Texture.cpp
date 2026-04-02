#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture.hpp"
#include "GLCheck.hpp"

#include <stdexcept>
#include <string>

Texture::Texture(std::string_view path)
{
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.data(), &m_width, &m_height, &m_channels, 0);
    if (!data)
        throw std::runtime_error(std::string("Cannot load texture: ") + path.data());

    GLenum format = (m_channels == 4) ? GL_RGBA : GL_RGB;

    GL_CHECK(glGenTextures(1, &m_id));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_id));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                          m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data));
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::bind(unsigned int slot) const
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_id));
}

void Texture::unbind() const
{
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}
