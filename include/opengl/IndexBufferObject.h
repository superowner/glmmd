#ifndef OPENGL_INDEX_BUFFER_OBJECT_H_
#define OPENGL_INDEX_BUFFER_OBJECT_H_

class IndexBufferObject
{
private:
    unsigned int m_id;
    unsigned int m_count;

public:
    IndexBufferObject();
    ~IndexBufferObject();

    IndexBufferObject(const IndexBufferObject &) = delete;
    IndexBufferObject &operator=(const IndexBufferObject &) = delete;

    void create(const unsigned int *data, unsigned int size);
    void destroy();
    inline unsigned int getCount() const { return m_count; }

    void bind() const;
    void unbind() const;
};
#endif