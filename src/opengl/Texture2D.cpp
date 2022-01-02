#include <glad/glad.h>
#include <opengl/Texture2D.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
Texture2D::Texture2D() : m_id(0), m_height(0), m_width(0), m_channels(0) {}
Texture2D::~Texture2D() { destroy(); }

void Texture2D::create(const std::string &filename)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_uc *data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, 0);
    if (data == nullptr)
    {
        data = (stbi_uc *)malloc(3 * sizeof(stbi_uc));
        m_width = 1;
        m_height = 1;
        m_channels = 3;
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = 0xff;
        std::cout << "Failed to load texture " << filename << std::endl;
    }
    if (m_channels == 3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, m_width, m_height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
    }
    else if (m_channels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, m_width, m_height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
    }
    else if (m_channels == 1)
    {
        stbi_uc *data3 = new stbi_uc[3 * m_width * m_height];
        for (int i = 0; i < m_width * m_height; i++)
        {
            data3[3 * i] = data[i];
            data3[3 * i + 1] = data[i];
            data3[3 * i + 2] = data[i];
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
        delete[] data3;
    }
    else if (m_channels == 2)
    {
        stbi_uc *data4 = new stbi_uc[4 * m_width * m_height];
        for (int i = 0; i < m_width * m_height; i++)
        {
            data4[4 * i] = data[2 * i];
            data4[4 * i + 1] = data[2 * i];
            data4[4 * i + 2] = data[2 * i];
            data4[4 * i + 3] = data[2 * i + 1];
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data4);
        delete[] data4;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void Texture2D::destroy()
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

void Texture2D::bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}