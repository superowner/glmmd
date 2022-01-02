#ifndef RENDERER_H_
#define RENDERER_H_

class ObjectBase
{
protected:
public:
    virtual void onUpdate(float deltaTime);
    virtual void onRender();
    virtual void onImGuiRender();
    virtual ~ObjectBase();
};

#endif