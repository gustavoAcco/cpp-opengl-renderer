#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string_view>

class Shader {
public:
    Shader(std::string_view vertexPath, std::string_view fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;

    void setMat4(std::string_view name, const glm::mat4& value) const;
    void setInt(std::string_view name, int value) const;
    void setFloat(std::string_view name, float value) const;

private:
    GLuint m_programID = 0;

    static GLuint compileShader(GLenum type, const char* source);
};
