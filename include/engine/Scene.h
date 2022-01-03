#ifndef SCENE_H_
#define SCENE_H_

#include <engine/Camera.h>
#include <engine/ObjectBase.h>
#include <engine/Event.h>
#include <engine/lighting/DirectionalLight.h>
#include <opengl/common.h>
#include <vector>
class Scene
{
private:
    Camera m_camera;
    DirectionalLight m_mainLight;
    std::vector<ObjectBase *> m_objectList;
    FrameBufferObject m_shadowMap;
    int m_width;
    int m_height;
    int m_shadowMapWidth;
    int m_shadowMapHeight;

public:
    EventQueue eventQueue;

public:
    Scene();

    void init(int width, int height, int shadowMapWidth, int shadowMapHeight);

    void handleEvent(float deltaTime);
    void onUpdate(float deltaTime);
    void onRenderShadowMap();
    void onRender();
    void onImGuiRender();

    void addObject(ObjectBase *pObj);
};

#endif