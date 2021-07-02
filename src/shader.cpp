#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "GL/glew.h"

#include "shader.h"

GLuint LoadShaders(const char* vert_file_path, const char* geo_file_path, const char* frag_file_path)
{
    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geometry_shader_handle = glCreateShader(GL_GEOMETRY_SHADER);

    // Load vertex shader from file
    std::string vertex_shader_code;
    std::ifstream vertex_shader_stream(vert_file_path, std::ios::in);
    if(vertex_shader_stream.is_open())
    {
        std::string line = "";
        while (getline(vertex_shader_stream, line))
        {
            vertex_shader_code += "\n" + line;
        }

        vertex_shader_stream.close();
    }
    else
    {
        printf("Can't open %s. \n", vert_file_path);
        return 0;
    }

    // Load fragment shader from file
    std::string fragment_shader_code;
    std::ifstream fragment_shader_stream(frag_file_path, std::ios::in);
    if(fragment_shader_stream.is_open()){
        std::string line = "";
        while (getline(fragment_shader_stream, line))
        {
            fragment_shader_code += "\n" + line;
        }
        fragment_shader_stream.close();
    }
    else
    {
        printf("Can't open %s. \n", frag_file_path);
        return 0;
    }

    // Load geometry shader from file
    std::string geometry_shader_code;
    std::ifstream geometry_shader_stream(geo_file_path, std::ios::in);
    if (geometry_shader_stream.is_open()){
        std::string line = "";
        while (getline(geometry_shader_stream, line))
        {
            geometry_shader_code += "\n" + line;
        }
        geometry_shader_stream.close();
    }
    else
    {
        printf("Can't open %s. \n", geo_file_path);
        return 0;
    }

    GLint compile_status = GL_FALSE;
    int compile_log_length;

    // Vertex Shader
    printf("Compiling shader : %s\n", vert_file_path);
    char const* vertex_src_ptr = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_handle, 1, &vertex_src_ptr , NULL);
    glCompileShader(vertex_shader_handle);
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(vertex_shader_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length+1);
        glGetShaderInfoLog(vertex_shader_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    // Fragment Shader
    printf("Compiling shader : %s\n", frag_file_path);
    char const* frag_src_ptr = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_handle, 1, &frag_src_ptr , NULL);
    glCompileShader(fragment_shader_handle);
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(fragment_shader_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if ( compile_log_length > 0 )
    {
        std::vector<char> msg(compile_log_length+1);
        glGetShaderInfoLog(fragment_shader_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    // Geometry Shader
    printf("Compiling shader : %s\n", geo_file_path);
    char const* geo_src_ptr = geometry_shader_code.c_str();
    glShaderSource(geometry_shader_handle, 1, &geo_src_ptr, NULL);
    glCompileShader(geometry_shader_handle);

    // Check Geometry Shader
    glGetShaderiv(geometry_shader_handle, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(geometry_shader_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length + 1);
        glGetShaderInfoLog(geometry_shader_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    printf("Linking program\n");
    GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glAttachShader(program_handle, geometry_shader_handle);
    glLinkProgram(program_handle);

    // Check the program
    glGetProgramiv(program_handle, GL_LINK_STATUS, &compile_status);
    glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length+1);
        glGetProgramInfoLog(program_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    glDeleteShader(vertex_shader_handle);
    glDeleteShader(fragment_shader_handle);
    glDeleteShader(geometry_shader_handle);

    return program_handle;
}

GLuint LoadShaders(const char* vert_file_path, const char* frag_file_path)
{
    // Lazy code duplication since this is just a sandbox...

    GLuint vertex_shader_handle = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_handle = glCreateShader(GL_FRAGMENT_SHADER);

    // Load vertex shader from file
    std::string vertex_shader_code;
    std::ifstream vertex_shader_stream(vert_file_path, std::ios::in);
    if (vertex_shader_stream.is_open())
    {
        std::string line = "";
        while (getline(vertex_shader_stream, line))
        {
            vertex_shader_code += "\n" + line;
        }

        vertex_shader_stream.close();
    }
    else
    {
        printf("Can't open %s. \n", vert_file_path);
        return 0;
    }

    // Load fragment shader from file
    std::string fragment_shader_code;
    std::ifstream fragment_shader_stream(frag_file_path, std::ios::in);
    if (fragment_shader_stream.is_open())
    {
        std::string line = "";
        while (getline(fragment_shader_stream, line))
        {
            fragment_shader_code += "\n" + line;
        }
        fragment_shader_stream.close();
    }
    else
    {
        printf("Can't open %s. \n", frag_file_path);
        return 0;
    }

    GLint compile_status = GL_FALSE;
    int compile_log_length;

    // Vertex Shader
    printf("Compiling shader : %s\n", vert_file_path);
    char const* vertex_src_ptr = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_handle, 1, &vertex_src_ptr, NULL);
    glCompileShader(vertex_shader_handle);
    glGetShaderiv(vertex_shader_handle, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(vertex_shader_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length + 1);
        glGetShaderInfoLog(vertex_shader_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    // Fragment Shader
    printf("Compiling shader : %s\n", frag_file_path);
    char const* frag_src_ptr = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_handle, 1, &frag_src_ptr, NULL);
    glCompileShader(fragment_shader_handle);
    glGetShaderiv(fragment_shader_handle, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(fragment_shader_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length + 1);
        glGetShaderInfoLog(fragment_shader_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    printf("Linking program\n");
    GLuint program_handle = glCreateProgram();
    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glLinkProgram(program_handle);

    // Check the program
    glGetProgramiv(program_handle, GL_LINK_STATUS, &compile_status);
    glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &compile_log_length);
    if (compile_log_length > 0)
    {
        std::vector<char> msg(compile_log_length + 1);
        glGetProgramInfoLog(program_handle, compile_log_length, NULL, &msg[0]);
        printf("%s\n", &msg[0]);
    }

    glDeleteShader(vertex_shader_handle);
    glDeleteShader(fragment_shader_handle);

    return program_handle;
}
