#ifndef DIRECTIONAL_LIGHT_H_
#define DIRECTIONAL_LIGHT_H_

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 dir;

    float diffuse[3];
    float ambient[3];
    void setDiffuse(float r, float g, float b)
    {
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
    }
    void setAmbient(float r, float g, float b)
    {
        ambient[0] = r;
        ambient[1] = g;
        ambient[2] = b;
    }
};

#endif