#include "Shader.hpp"
#include "GLCheck.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static std::string readFile(std::string_view path)
{
    std::ifstream file(path.data());
    if (!file.is_open())
        throw std::runtime_error(std::string("Cannot open shader file: ") + path.data());
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    GL_CHECK(glShaderSource(shader, 1, &source, nullptr));
    GL_CHECK(glCompileShader(shader));

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<std::size_t>(logLen));
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compile error: ") + log.data());
    }
    return shader;
}

Shader::Shader(std::string_view vertexPath, std::string_view fragmentPath)
{
    const std::string vertSrc  = readFile(vertexPath);
    const std::string fragSrc  = readFile(fragmentPath);

    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertSrc.c_str());
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());

    m_programID = glCreateProgram();
    GL_CHECK(glAttachShader(m_programID, vert));
    GL_CHECK(glAttachShader(m_programID, frag));
    GL_CHECK(glLinkProgram(m_programID));

    GLint success = 0;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<std::size_t>(logLen));
        glGetProgramInfoLog(m_programID, logLen, nullptr, log.data());
        glDeleteShader(vert);
        glDeleteShader(frag);
        glDeleteProgram(m_programID);
        throw std::runtime_error(std::string("Shader link error: ") + log.data());
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader()
{
    glDeleteProgram(m_programID);
}

void Shader::use() const
{
    GL_CHECK(glUseProgram(m_programID));
}

void Shader::setMat4(std::string_view name, const glm::mat4& value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.data());
    GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::setInt(std::string_view name, int value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.data());
    GL_CHECK(glUniform1i(loc, value));
}

void Shader::setFloat(std::string_view name, float value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.data());
    GL_CHECK(glUniform1f(loc, value));
}
