#include <glad/glad.h>
#include <opengl/VertexBufferObject.h>

#include <cassert>

VertexBufferObject::VertexBufferObject() : m_id(0) {}
VertexBufferObject::~VertexBufferObject() { destroy(); }

void VertexBufferObject::create(const void *data, unsigned int size)
{
    destroy();
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBufferObject::destroy()
{
    if (m_id != 0)
    {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

void VertexBufferObject::bind() const
{
    assert(m_id != 0);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBufferObject::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
