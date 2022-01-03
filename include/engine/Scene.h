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
    std::vector<ObjectBase *> m_objectList;
    std::vector<Shader *> m_shaderList;
    DirectionalLight m_mainLight;

public:
    EventQueue eventQueue;

public:
    Scene(int width, int height);

    void onUpdate(float deltaTime);
    void onRender();
    void onImGuiRender();

    void addShader(Shader &);
    void addObject(ObjectBase &);
};

#endif