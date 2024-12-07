#include "glad/gl.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>

static GLchar info[512] = {0};

const char* getError()
{
    // clang-format off
    switch(glGetError())
    {
    case GL_NO_ERROR:                       return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
    case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
    default:                                return "";

    }
    // clang-format on
}

std::vector<char> readMyFile(const char* filename)
{
    std::ifstream   file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    return buffer;
}

GLuint createShader(GLenum shader_type, const char* filename)
{
    GLuint shader = glCreateShader(shader_type);

    auto          content = readMyFile(filename);
    const GLchar* buffer  = content.data();
    const GLint   size    = content.size();

    glShaderSource(shader, 1, &buffer, &size);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE)
    {
        glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
        std::cout << "Shader compilation:\n"
                  << "Status: " << status << "\n"
                  << "Error: " << getError() << "\n"
                  << "Message: " << info << std::endl;
    }

    return shader;
}

struct Shader
{
    GLuint program  = 0;
    GLuint vertex   = 0;
    GLuint fragment = 0;

    Shader()
    {
        program = glCreateProgram();
    }

    ~Shader()
    {
        glDeleteProgram(program);
    }

    void vertexShader(const char* filename)
    {
        vertex = createShader(GL_VERTEX_SHADER, filename);
        glAttachShader(program, vertex);
    }

    void fragmentShader(const char* filename)
    {
        fragment = createShader(GL_FRAGMENT_SHADER, filename);
        glAttachShader(program, fragment);
    }

    void link()
    {
        glLinkProgram(program);

        GLint status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);

        if(status == GL_FALSE)
        {
            glGetProgramInfoLog(program, sizeof(info), nullptr, info);

            std::cout << "Program linking:\n"
                      << "Status: " << status << "\n"
                      << "Error: " << getError() << "\n"
                      << "Message: " << info << std::endl;
        }
    }

    void validate()
    {
        glValidateProgram(program);

        GLint status = 0;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

        if(status == GL_FALSE)
        {
            glGetProgramInfoLog(program, sizeof(info), nullptr, info);
            std::cout << "Program validation:\n"
                      << "Status: " << status << "\n"
                      << "Error: " << getError() << "\n"
                      << "Message: " << info << std::endl;
        }
    }

    void activate()
    {
        glUseProgram(program);
    }

    template<typename T>
    void set(const char* name, T t)
    {
        GLint location = glGetUniformLocation(program, name);
        set(location, t);
    }

    void set(GLint location, int value)
    {
        glUniform1i(location, value);
    }

    void set(GLint location, float value)
    {
        glUniform1f(location, value);
    }

    void set(GLint location, glm::vec2& value)
    {
        glUniform2fv(location, 1, &value[0]);
    }

    void set(GLint location, glm::vec3& value)
    {
        glUniform3fv(location, 1, &value[0]);
    }

    void set(GLint location, glm::mat3& value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
    }

    void set(GLint location, glm::mat4& value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
};
