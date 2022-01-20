#include <glad/glad.h>
#include <opengl/Shader.h>
#include <fstream>
#include <sstream>
#include <utility>

Shader::Shader() : m_id(0) {}

void Shader::create(const std::string &vertPath, const std::string &fragPath)
{
    destroy();
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
        throw std::make_pair<ShaderErr, std::string>(FILE_LOAD_ERR, "");
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
        throw std::make_pair<ShaderErr, std::string>(VERT_COMPILE_ERR, infoLog);
    };

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc_c, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        throw std::make_pair<ShaderErr, std::string>(FRAG_COMPILE_ERR, infoLog);
    };

    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        throw std::make_pair<ShaderErr, std::string>(LINKING_ERR, infoLog);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::use() const { glUseProgram(m_id); }
unsigned int Shader::getId() const { return m_id; }
void Shader::destroy()
{
    if (m_id != 0)
    {
        glDeleteProgram(m_id);
        m_id = 0;
    }
}
Shader::~Shader() { destroy(); }

void Shader::setUniform1i(const char *name, int n) const
{
    glUniform1i(glGetUniformLocation(m_id, name), n);
}

void Shader::setUniformMatrix4fv(const char *name, bool transpose, const glm::mat4 m) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, transpose, &m[0][0]);
}

void Shader::setUniformMatrix4fv(const char *name, unsigned int count, bool transpose, const float *ptr) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), count, transpose, ptr);
}

void Shader::setUniform3fv(const char *name, const glm::vec3 v) const
{
    glUniform3fv(glGetUniformLocation(m_id, name), 1, &v.x);
}

void Shader::setUniform1f(const char *name, float f) const
{
    glUniform1f(glGetUniformLocation(m_id, name), f);
}

void Shader::setUniform4fv(const char *name, const glm::vec4 v) const
{
    glUniform4fv(glGetUniformLocation(m_id, name), 1, &v.x);
}