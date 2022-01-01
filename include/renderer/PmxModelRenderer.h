#ifndef PMX_MODEL_RENDERER_H_
#define PMX_MODEL_RENDERER_H_

#include <renderer/Renderer.h>
#include <pmx/PmxModel.h>

class PmxModelRenderer : public Renderer
{
private:
    pmx::Model *m_pModel;
    VertexArrayObject m_VAO;
    VertexBufferObject m_VBO;
    IndexBufferObject m_IBO;

public:
    PmxModelRenderer(pmx::Model *);
    void OnRender(const Shader &) const;
};

#endif
