#include <glad/glad.h>
#include <opengl/RenderBufferObject.h>
RenderBufferObject::RenderBufferObject() : m_id(0){};
RenderBufferObject::~RenderBufferObject() { destroy(); };

void RenderBufferObject::create(int width, int height)
{
    glGenRenderbuffers(1, &m_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void RenderBufferObject::destroy()
{
}
void RenderBufferObject::bind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
}
void RenderBufferObject::unbind() const
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

unsigned int RenderBufferObject::id() const { return m_id; }