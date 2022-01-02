#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
    float yfov;
    float near;
    float far;
    glm::vec3 pos;
    glm::vec3 target;
    glm::vec3 dir; //inverse
    glm::vec3 up;
    glm::vec3 right;

    Camera(float _yfov = 45.0f, float _near = 0.1f, float _far = 100.0f, glm::vec3 _pos = glm::vec3(0.0f, 16.0f, -24.0f), glm::vec3 _target = glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjMatrix(float aspect /*width : height*/) const;
    glm::mat4 getOrthoProjMatrix(float left, float right, float bottom, float top) const;
};

#endif
