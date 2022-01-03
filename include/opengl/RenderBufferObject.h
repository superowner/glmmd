#ifndef RENDER_BUFFER_OBJECT_H_
#define RENDER_BUFFER_OBJECT_H_

class RenderBufferObject
{
private:
    unsigned int m_id;

public:
    RenderBufferObject();
    ~RenderBufferObject();

    void create(int width, int height);
    void destroy();

    void bind() const;
    void unbind() const;

    unsigned int id() const;
};
#endif
