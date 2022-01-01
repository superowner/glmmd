#ifndef OPENGL_SHADER_H_
#define OPENGL_SHADER_H_

#include <string>

class Shader
{
private:
    unsigned int m_id;

public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    void use() const;
    void destroy();
};

#endif