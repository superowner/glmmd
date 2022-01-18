#include <engine/Camera.h>
Camera::Camera(float _yfov, float _near, float _far, glm::vec3 _pos, glm::vec3 _target, glm::vec3 _up)
    : yfov(_yfov), near(_near), far(_far), pos(_pos), target(_target)
{

    dir = glm::normalize(pos - target);
    up = glm::normalize(_up - glm::dot(_up, dir) * dir);
    right = glm::cross(up, dir);
}
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(pos, pos - dir, up);
}
glm::mat4 Camera::getProjMatrix() const
{
    return glm::perspective(glm::radians(yfov), aspect, near, far);
}
glm::mat4 Camera::getOrthoProjMatrix(float left, float right, float bottom, float top) const
{
    return glm::ortho(left, right, bottom, top, near, far);
}

void Camera::translate(const glm::vec3 &displacement)
{
    pos += displacement;
}