#ifndef OFFSCREEN_RENDERER_H_
#define OFFSCREEN_RENDERER_H_

#include <opengl/common.h>

class OffscreenRenderer
{
private:
    int m_width;
    int m_height;
    int m_samples;
    FrameBufferObject m_FBO;
    FrameBufferObject m_intermediateFBO;
    VertexArrayObject m_quadVAO;
    VertexBufferObject m_quadVBO;
    Shader *m_pScreenShader;

    bool m_enableHdr;
    float m_hdrExposure;

public:
    OffscreenRenderer(int width, int height, int sample, Shader *pScreenShader);
    ~OffscreenRenderer();

    void begin() const;
    void onImGui();
    void end() const;
};

#endif
