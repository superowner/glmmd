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
    void create(int width, int height);
    void createDepthMap(int width, int height);
    void createFloatBuffer(int width, int height);
    void bindData(const float *);

    void destroy();
    void bind() const;
    void bind(unsigned int unit) const;
    void unbind() const;

    unsigned int id() const;
};

#endif
