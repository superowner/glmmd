#ifndef FRAME_BUFFER_OBJECT_H_
#define FRAME_BUFFER_OBJECT_H_
#include <opengl/RenderBufferObject.h>
#include <opengl/Texture2D.h>
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

    void create(int width, int height, GLenum internalFmt = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
    void createDepthBuffer(int width, int height);
    void createMultiSample(int width, int height, int sample, GLenum fmt);
    void destroy();

    void bind() const;
    void bindRead() const;
    void bindDraw() const;
    void unbind() const;

    const Texture2D &tex() const;
};

#endif