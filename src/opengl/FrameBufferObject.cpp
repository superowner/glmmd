#include <glad/glad.h>
#include <opengl/FrameBufferObject.h>
FrameBufferObject::FrameBufferObject()
    : m_id(0), m_width(0), m_height(0)
{
}
FrameBufferObject::~FrameBufferObject() { destroy(); }

void FrameBufferObject::create(int width, int height)
{
    destroy();
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    m_tex.create(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex.id(), 0);

    m_RBO.create(width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO.id());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::createDepthBuffer(int width, int height)
{
    destroy();
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    m_tex.createDepthMap(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_tex.id(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::createMultiSample(int width, int height, int sample)
{
    destroy();
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    m_tex.createMultiSample(width, height, sample);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_tex.id(), 0);

    m_RBO.createMultiSample(width, height, sample);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO.id());
}

void FrameBufferObject::destroy()
{
    if (m_id != 0)
    {
        glDeleteFramebuffers(1, &m_id);
        m_id = 0;
    }
}

void FrameBufferObject::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void FrameBufferObject::bindRead() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
}

void FrameBufferObject::bindDraw() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
}

void FrameBufferObject::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const Texture2D &FrameBufferObject::tex() const
{
    return m_tex;
}