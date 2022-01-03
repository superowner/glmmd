#ifndef FRAME_BUFFER_OBJECT_H_
#define FRAME_BUFFER_OBJECT_H_
#include <opengl/Texture2D.h>
#include <opengl/RenderBufferObject.h>
class FrameBufferObject
{
private:
    unsigned int m_id;
    int m_width;
    int m_height;
    Texture2D m_tex;
    RenderBufferObject m_RBO;

public:
    FrameBufferObject();
    ~FrameBufferObject();

    void create(int width, int height);
    void destroy();

    void bind() const;
    void unbind() const;

    const Texture2D &tex() const;
};

#endif