#include <glad/glad.h>
#include <opengl/FrameBufferObject.h>
#include <iostream>
FrameBufferObject::FrameBufferObject()
    : m_id(0), m_width(0), m_height(0)
{
}
FrameBufferObject::~FrameBufferObject() { destroy(); }

void FrameBufferObject::create(int width, int height)
{
    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    m_tex.create(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex.id(), 0);

    m_RBO.create(width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO.id());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void FrameBufferObject::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const Texture2D &FrameBufferObject::tex() const
{
    return m_tex;
}