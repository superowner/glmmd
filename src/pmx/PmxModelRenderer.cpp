#include <pmx/PmxModelRenderer.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

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
        if (mat.bitFlag & 0x01)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        m_IBOList[i].bind();
        glDrawElements(GL_TRIANGLES, m_IBOList[i].getCount(), GL_UNSIGNED_INT, 0);
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
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

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
        m_texList[i].createFromFile(m_pModel->texturePath[i]);

    // load default toon
    for (unsigned int i = 0; i < 10; ++i)
        m_defaultToon[i].createFromFile(std::string("../res/toon/toon") + (i < 9 ? "0" : "") + std::to_string(i + 1) + ".bmp");
}

void PmxModelRenderer::onImGuiRender()
{
}