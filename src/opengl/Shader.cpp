#include <glad/glad.h>
#include <opengl/Shader.h>
#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string &vertPath, const std::string &fragPath)
{
    std::string vertSrc, fragSrc;
    std::ifstream vertFile, fragFile;
    const char *vertSrc_c, *fragSrc_c;
    vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vertFile.open(vertPath);
        fragFile.open(fragPath);
        std::stringstream vss, fss;
        vss << vertFile.rdbuf();
        fss << fragFile.rdbuf();
        vertFile.close();
        fragFile.close();
        vertSrc = vss.str();
        vertSrc_c = vertSrc.c_str();
        fragSrc = fss.str();
        fragSrc_c = fragSrc.c_str();
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "failed to load shader from file\n";
    }

    unsigned int vert, frag;
    int success;
    char infoLog[512];

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc_c, NULL);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        std::cout << "vertex shader compile error\n"
                  << infoLog << std::endl;
    };

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc_c, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "fragment shader compile error\n"
                  << infoLog << std::endl;
    };

    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout << "failed to link shader\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::use() const { glUseProgram(m_id); }
void Shader::destroy()
{
    if (m_id != 0)
    {
        glDeleteProgram(m_id);
        m_id = 0;
    }
}
Shader::~Shader() { destroy(); }
