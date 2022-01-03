#include <engine/PmxModelRenderer.h>

PmxModelRenderer::PmxModelRenderer(pmx::Model *pModel, Shader *pShader)
    : m_pModel(pModel), m_pShader(pShader)
{
    assert(m_pModel != nullptr && m_pShader != nullptr);

    m_VAO.create();
    m_VAO.bind();

    unsigned int stride = 8;

    float *vertices = new float[m_pModel->vertices.size() * stride];

    for (size_t i = 0; i < m_pModel->vertices.size(); ++i)
    {
        vertices[stride * i + 0] = m_pModel->vertices[i].position.x;
        vertices[stride * i + 1] = m_pModel->vertices[i].position.y;
        vertices[stride * i + 2] = m_pModel->vertices[i].position.z;

        vertices[stride * i + 3] = m_pModel->vertices[i].normal.x;
        vertices[stride * i + 4] = m_pModel->vertices[i].normal.y;
        vertices[stride * i + 5] = m_pModel->vertices[i].normal.z;

        vertices[stride * i + 6] = m_pModel->vertices[i].uv.x;
        vertices[stride * i + 7] = m_pModel->vertices[i].uv.y;
    }

    m_VBO.create(vertices, sizeof(float) * m_pModel->vertices.size() * stride);
    delete[] vertices;

    m_IBOList.resize(m_pModel->materials.size());

    unsigned int indexOffset = 0u;
    for (unsigned int i = 0; i < m_IBOList.size(); ++i)
    {
        m_IBOList[i].create(&m_pModel->indices[indexOffset], sizeof(unsigned int) * m_pModel->materials[i].indexCount);
        indexOffset += m_pModel->materials[i].indexCount;
    }

    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    layout.push(GL_FLOAT, 3);
    layout.push(GL_FLOAT, 2);
    m_VAO.addBuffer(m_VBO, layout);
    m_VBO.unbind();
    m_VAO.unbind();

    m_texList.resize(m_pModel->texturePath.size());
    for (unsigned int i = 0; i < m_texList.size(); ++i)
        m_texList[i].create(m_pModel->texturePath[i]);
}

void PmxModelRenderer::onRender()
{
    m_pShader->use();
    m_VAO.bind();
    glCullFace(GL_FRONT);
    for (unsigned int i = 0; i < m_IBOList.size(); ++i)
    {
        if (m_pModel->materials[i].bitFlag & 0x01)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        m_texList[m_pModel->materials[i].diffuseTexId].bind(0);
        m_texList[m_pModel->materials[i].sphereTexId].bind(1);
        m_IBOList[i].bind();
        glDrawElements(GL_TRIANGLES, m_IBOList[i].getCount(), GL_UNSIGNED_INT, 0);
    }
}