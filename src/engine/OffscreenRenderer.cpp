#include <engine/OffscreenRenderer.h>
#include <imgui/imgui.h>
OffscreenRenderer::OffscreenRenderer(int width, int height, int sample, Shader *pScreenShader)
    : m_width(width), m_height(height), m_samples(sample),
      m_pScreenShader(pScreenShader), m_enableHdr(true), m_hdrExposure(1.0f)
{
    assert(pScreenShader != nullptr);
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
        m_FBO.create(width, height, GL_RGBA16F, GL_FLOAT);
    else
    {
        m_FBO.createMultiSample(width, height, sample, GL_RGBA16F);
        m_intermediateFBO.create(width, height, GL_RGBA16F, GL_FLOAT);
    }
}
OffscreenRenderer::~OffscreenRenderer() {}

void OffscreenRenderer::begin() const
{
    m_FBO.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void OffscreenRenderer::onImGui()
{
    ImGui::Begin("Screen Effect Panel");
    ImGui::Checkbox("Enable Hdr", &m_enableHdr);
    ImGui::SliderFloat("Hdr Exposure", &m_hdrExposure, 0.0f, 16.0f);

    ImGui::End();
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pScreenShader->use();
    m_quadVAO.bind();
    if (m_samples > 1)
        m_intermediateFBO.tex().bind(0);
    else
        m_FBO.tex().bind(0);
    m_pScreenShader->setUniform1i("screenTexture", 0);
    m_pScreenShader->setUniform1i("enableHdr", m_enableHdr);
    m_pScreenShader->setUniform1f("hdrExposure", m_hdrExposure);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}