#include <engine/OffscreenRenderer.h>
OffscreenRenderer::OffscreenRenderer(int width, int height, const std::string &scrVertShaderPath, const std::string &scrFragShaderPath)
    : m_screenShader(scrVertShaderPath, scrFragShaderPath)
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

    m_FBO.create(width, height);
}
OffscreenRenderer::~OffscreenRenderer() {}

void OffscreenRenderer::begin() const
{
    m_FBO.bind();
    glEnable(GL_DEPTH_TEST);
}

void OffscreenRenderer::end() const
{
    m_FBO.unbind();
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_screenShader.use();
    m_quadVAO.bind();
    m_FBO.tex().bind(0);
    m_screenShader.setUniform1i("screenTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}