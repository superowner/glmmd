#include <renderer/PmxModelRenderer.h>
PmxModelRenderer::PmxModelRenderer(pmx::Model *pModel)
    : m_pModel(pModel)
{
    assert(pModel != nullptr);

    m_VAO.create();
    m_VAO.bind();

    float *vertices = new float[m_pModel->vertices.size() * 3];

    for (size_t i = 0; i < m_pModel->vertices.size(); ++i)
    {
        vertices[3 * i + 0] = m_pModel->vertices[i].position.x * 0.1;
        vertices[3 * i + 1] = m_pModel->vertices[i].position.y * 0.1;
        vertices[3 * i + 2] = m_pModel->vertices[i].position.z * 0.1;
    }

    m_VBO.create(vertices, sizeof(float) * m_pModel->vertices.size() * 3);
    delete[] vertices;

    m_IBO.create(&m_pModel->indices[0], sizeof(unsigned int) * m_pModel->indices.size());

    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    m_VAO.addBuffer(m_VBO, layout);
    m_VBO.unbind();
    m_VAO.unbind();
    m_IBO.unbind();
}

void PmxModelRenderer::OnRender(const Shader &shader) const
{
    shader.use();
    m_VAO.bind();
    m_IBO.bind();
    glDrawElements(GL_TRIANGLES, m_IBO.getCount(), GL_UNSIGNED_INT, 0);
}