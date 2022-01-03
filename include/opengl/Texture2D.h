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
    void create(const std::string &filename);
    void create(int width, int height);

    void destroy();
    void bind(unsigned int unit) const;
    void unbind() const;

    unsigned int id() const;
};

#endif
