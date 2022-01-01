#ifndef OPENGL_VERTEX_ARRAY_OBJECT_H_
#define OPENGL_VERTEX_ARRAY_OBJECT_H_

#include <glad/glad.h>
#include <opengl/VertexBufferObject.h>

#include <vector>
#include <cassert>

class VertexBufferLayout
{
private:
    std::vector<std::pair<unsigned int, unsigned int>> m_elements; // {type, count}
    unsigned int m_stride;

public:
    static unsigned int getSize(unsigned int ty)
    {
        switch (ty)
        {
        case GL_FLOAT:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        default:
            assert(0);
        }
        return 0;
    }

    VertexBufferLayout() : m_stride(0) {}

    unsigned int getElementCount() const { return m_elements.size(); }
    unsigned int getStride() const { return m_stride; }

    unsigned int getType(size_t i) const { return m_elements[i].first; }
    unsigned int getCount(size_t i) const { return m_elements[i].second; }

    void push(unsigned int ty, unsigned int count)
    {
        m_elements.push_back({ty, count});
        m_stride += getSize(ty) * count;
    }
};

class VertexArrayObject
{
private:
    unsigned int m_id;

public:
    VertexArrayObject();
    ~VertexArrayObject();

    VertexArrayObject(const VertexArrayObject &) = delete;
    VertexArrayObject &operator=(const VertexArrayObject &) = delete;

    void create();
    void addBuffer(const VertexBufferObject &vbo, const VertexBufferLayout &layout);
    void destroy();

    void bind() const;
    void unbind() const;
};

#endif