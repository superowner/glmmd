#ifndef PMX_MODEL_RENDERER_H_
#define PMX_MODEL_RENDERER_H_

#include <engine/ObjectBase.h>
#include <pmx/PmxModel.h>
#include <opengl/common.h>
class PmxModelRenderer : public ObjectBase
{
private:
    pmx::Model *m_pModel;
    VertexArrayObject m_VAO;
    VertexBufferObject m_VBO;
    std::vector<IndexBufferObject> m_IBOList;
    std::vector<Texture2D> m_texList;
    Texture2D m_defaultToon[10];

    float m_rotY;

public:
    PmxModelRenderer(pmx::Model *, Shader *, Shader *);
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onRenderShadowMap() override;
    void onImGuiRender() override;
};

#endif
