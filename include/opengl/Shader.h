#ifndef OPENGL_SHADER_H_
#define OPENGL_SHADER_H_

#include <glm/glm.hpp>
#include <string>

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
    void setUniformMatrix4fv(const char *name, bool transpose, const glm::mat4 m) const;
    void setUniformMatrix4fv(const char *name, unsigned int count, bool transpose, const float *) const;
    void setUniform3fv(const char *name, const glm::vec3 v) const;
    void setUniform1f(const char *name, float f) const;
    void setUniform4fv(const char *name, const glm::vec4 v) const;
};

#endif