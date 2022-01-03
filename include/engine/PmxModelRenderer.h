#ifndef PMX_MODEL_RENDERER_H_
#define PMX_MODEL_RENDERER_H_

#include <engine/ObjectBase.h>
#include <pmx/PmxModel.h>
#include <opengl/common.h>
class PmxModelRenderer : public ObjectBase
{
private:
    pmx::Model *m_pModel;
    Shader *m_pShader;
    VertexArrayObject m_VAO;
    VertexBufferObject m_VBO;
    std::vector<IndexBufferObject> m_IBOList;
    std::vector<Texture2D> m_texList;

public:
    PmxModelRenderer(pmx::Model *, Shader *);
    void onRender() override;
};

#endif
