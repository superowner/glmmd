#include <engine/OffscreenRenderer.h>
OffscreenRenderer::OffscreenRenderer(int width, int height, int sample, Shader *pScreenShader)
    : m_width(width), m_height(height), m_samples(sample), m_pScreenShader(pScreenShader)
{
    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};
    m_quadVAO.create();
    m_quadVAO.bind();
    m_quadVBO.create(quadVertices, sizeof(quadVertices));
    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 2);
    layout.push(GL_FLOAT, 2);
    m_quadVAO.addBuffer(m_quadVBO, layout);

    if (sample < 2)
        m_FBO.create(width, height);
    else
    {
        m_FBO.createMultiSample(width, height, sample);
        m_intermediateFBO.create(width, height);
    }
}
OffscreenRenderer::~OffscreenRenderer() {}

void OffscreenRenderer::begin() const
{
    m_FBO.bind();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void OffscreenRenderer::end() const
{
    if (m_samples > 1)
    {
        m_FBO.bindRead();
        m_intermediateFBO.bindDraw();
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    m_FBO.unbind();
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pScreenShader->use();
    m_quadVAO.bind();
    if (m_samples > 1)
        m_intermediateFBO.tex().bind(0);
    else
        m_FBO.tex().bind(0);
    m_pScreenShader->setUniform1i("screenTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}