#ifndef RENDERER_H_
#define RENDERER_H_
#include <opengl/Shader.h>
class ObjectBase
{
protected:
    Shader *m_mainShader;
    Shader *m_depthShader;

public:
    ObjectBase(Shader *shader, Shader *depthShader = nullptr);
    virtual void onUpdate(float deltaTime);
    virtual void onRenderShadowMap();
    virtual void onRender();
    virtual void onImGuiRender();

    Shader *mainShader();
    Shader *depthShader();
    virtual ~ObjectBase();
};

#endif