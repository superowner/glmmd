#ifndef OFFSCREEN_RENDERER_H_
#define OFFSCREEN_RENDERER_H_

#include <opengl/common.h>

class OffscreenRenderer
{
private:
    int m_width;
    int m_height;
    FrameBufferObject m_FBO;
    VertexArrayObject m_quadVAO;
    VertexBufferObject m_quadVBO;
    Shader m_screenShader;

public:
    OffscreenRenderer(int width, int height, const std::string &scrVertShaderPath, const std::string &scrFragShaderPath);
    ~OffscreenRenderer();

    void begin() const;
    void end() const; 
};

#endif
