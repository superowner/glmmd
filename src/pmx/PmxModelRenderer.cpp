#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <pmx/PmxModelRenderer.h>
void PmxModelRenderer::onUpdate(float deltaTime)
{
}

void PmxModelRenderer::onRenderShadowMap()
{
    m_depthShader->use();
    m_VAO.bind();
    for (unsigned int i = 0; i < m_IBOList.size(); ++i)
    {
        const auto &mat = m_pModel->materials[i];
        if (mat.bitFlag & (0x02 | 0x04 | 0x08))
        {
            if (mat.bitFlag & 0x01)
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);
            m_IBOList[i].bind();
            glDrawElements(GL_TRIANGLES, m_IBOList[i].getCount(), GL_UNSIGNED_INT, 0);
        }
    }
}

void PmxModelRenderer::onRender()
{
    m_mainShader->use();
    m_VAO.bind();
    for (unsigned int i = 0; i < m_IBOList.size(); ++i)
    {
        const auto &mat = m_pModel->materials[i];
        if (mat.bitFlag & 0x01)
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);

        m_mainShader->setUniform4fv("mat.diffuseColor", 1, mat.diffuse);
        m_mainShader->setUniform3fv("mat.specularColor", 1, mat.specular);
        m_mainShader->setUniform1f("mat.specIntensity", mat.specIntensity);
        m_mainShader->setUniform3fv("mat.ambientColor", 1, mat.ambient);

        m_mainShader->setUniform4fv("mat.diffuse", 1, mat.edgeColor);
        m_mainShader->setUniform1f("mat.specularIntensity", mat.edgeSize);

        if (0 <= mat.diffuseTexId && mat.diffuseTexId < m_texList.size())
        {
            m_mainShader->setUniform1i("mat.hasDiffuseTex", 1);
            m_texList[mat.diffuseTexId].bind(0);
            m_mainShader->setUniform1i("mat.diffuseTex", 0);
        }
        else
            m_mainShader->setUniform1i("mat.hasDiffuseTex", 0);

        if (0 <= mat.sphereTexId && mat.sphereTexId < m_texList.size())
        {
            m_mainShader->setUniform1i("mat.sphereTexMode", mat.sphereMode);
            m_texList[mat.sphereTexId].bind(1);
            m_mainShader->setUniform1i("mat.sphereTex", 1);
        }
        else
            m_mainShader->setUniform1i("mat.sphereTexMode", 0);

        if (mat.sharedToonFlag)
        {
            m_mainShader->setUniform1i("mat.hasToon", 1);
            m_defaultToon[mat.toonTexId].bind(2);
            m_mainShader->setUniform1i("mat.toonTex", 2);
        }
        else if (0 <= mat.toonTexId && mat.toonTexId < m_texList.size())
        {
            m_mainShader->setUniform1i("mat.hasToon", 1);
            m_texList[mat.toonTexId].bind(2);
            m_mainShader->setUniform1i("mat.toonTex", 2);
        }
        else
            m_mainShader->setUniform1i("mat.hasToon", 0);

        m_IBOList[i].bind();
        glDrawElements(GL_TRIANGLES, m_IBOList[i].getCount(), GL_UNSIGNED_INT, 0);
    }
}
PmxModelRenderer::PmxModelRenderer(pmx::Model *pModel, Shader *pShader, Shader *pDepthShader)
    : ObjectBase(pShader, pDepthShader), m_pModel(pModel)
{
    assert(m_pModel != nullptr);

    m_VAO.create();
    m_VAO.bind();

    const unsigned int stride = 8;

    auto &vl = m_pModel->vertices;
    float *vertices = new float[vl.size() * stride];

    size_t j = 0;
    for (size_t i = 0; i < vl.size(); ++i, j += stride)
    {
        vertices[j + 0] = vl[i].position.x;
        vertices[j + 1] = vl[i].position.y;
        vertices[j + 2] = vl[i].position.z;

        vertices[j + 3] = vl[i].normal.x;
        vertices[j + 4] = vl[i].normal.y;
        vertices[j + 5] = vl[i].normal.z;

        vertices[j + 6] = vl[i].uv.x;
        vertices[j + 7] = vl[i].uv.y;
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
        m_texList[i].createFromFile(m_pModel->texturePath[i]);

    // load default toon
    for (unsigned int i = 0; i < 10; ++i)
        m_defaultToon[i].createFromFile(std::string("../res/toon/toon") + (i < 9 ? "0" : "") +
                                        std::to_string(i + 1) + ".bmp");
}

void PmxModelRenderer::onImGuiRender()
{
}