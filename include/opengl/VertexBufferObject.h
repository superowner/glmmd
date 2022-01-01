#ifndef OPENGL_VERTEX_BUFFER_OBJECT_H_
#define OPENGL_VERTEX_BUFFER_OBJECT_H_

class VertexBufferObject
{
private:
    unsigned int m_id;

public:
    VertexBufferObject();
    ~VertexBufferObject();

    VertexBufferObject(const VertexBufferObject &) = delete;
    VertexBufferObject &operator=(const VertexBufferObject &) = delete;

    void create(const void *data, unsigned int size);
    void destroy();

    void bind() const;
    void unbind() const;
};

#endif
