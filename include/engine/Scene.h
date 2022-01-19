#ifndef SCENE_H_
#define SCENE_H_

#include <engine/Camera.h>
#include <engine/Event.h>
#include <engine/ObjectBase.h>
#include <engine/lighting/DirectionalLight.h>
#include <opengl/common.h>
#include <vector>
class Scene
{
private:
    Camera m_camera;
    Camera m_lightCamera;
    glm::mat4 m_lightSpaceMatrix;
    FrameBufferObject m_shadowMap;
    DirectionalLight m_mainLight;
    std::vector<ObjectBase *> m_objectList;
    int m_width;
    int m_height;
    int m_shadowMapWidth;
    int m_shadowMapHeight;
    float m_lightCamWidth;
    float m_lightCamHeight;
    float m_lightCamNear;
    float m_lightCamFar;

public:
    EventQueue eventQueue;

public:
    Scene();

    void init(int width, int height, int shadowMapWidth, int shadowMapHeight,
              float lightCamWidth, float lightCamHeight, float lightCamNear, float lightCamFar);

    void handleEvent(float deltaTime);
    void onUpdate(float deltaTime);
    void onRenderShadowMap();
    void onRender();
    void onImGuiRender();

    void addObject(ObjectBase *pObj);
};

#endif