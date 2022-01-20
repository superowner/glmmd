#ifndef TEXTURE_2D_H_
#define TEXTURE_2D_H_
#include <string>

class Texture2D
{
private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_channels;

public:
    Texture2D();
    ~Texture2D();
    // Texture2D(const Texture2D &) = delete;
    // Texture2D &operator=(const Texture2D &) = delete;
    void createFromFile(const std::string &filename);
    void create(int width, int height, GLenum internalFmt = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
    void createDepthMap(int width, int height);
    void createMultiSample(int width, int height, int sample, GLenum fmt = GL_RGB);
    void createFloatBuffer(int width, int height);

    void fillFloatData(const float *data);
    void destroy();
    void bind() const;
    void bind(unsigned int unit) const;
    void unbind() const;

    inline unsigned int id() const { return m_id; }
    inline unsigned int channels() const { return m_channels; }
    inline unsigned int samples() const { return m_channels; }
};

#endif
