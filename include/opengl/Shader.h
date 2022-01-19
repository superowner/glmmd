#ifndef OPENGL_SHADER_H_
#define OPENGL_SHADER_H_

#include <string>
#include <glm/glm.hpp>

enum ShaderErr
{
    FILE_LOAD_ERR,
    VERT_COMPILE_ERR,
    FRAG_COMPILE_ERR,
    LINKING_ERR
};


class Shader
{
private:
    unsigned int m_id;

public:
    Shader();
    ~Shader();

    void create(const std::string &vertPath, const std::string &fragPath);

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    unsigned int getId() const;

    void use() const;
    void destroy();

    void setUniform1i(const char *name, int n) const;
    void setUniformMatrix4fv(const char *name, unsigned int count, bool transpose, const glm::mat4 m) const;
    void setUniform3fv(const char *name, unsigned int count, const glm::vec3 v) const;
    void setUniform1f(const char *name, float f) const;
    void setUniform4fv(const char *name, unsigned int count, const glm::vec4 v) const;
};

#endif