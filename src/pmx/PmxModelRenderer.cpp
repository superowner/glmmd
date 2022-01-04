#include <pmx/PmxModelRenderer.h>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
void PmxModelRenderer::onUpdate(float deltaTime)
{
    currentTime += deltaTime * 30;
    if (m_pAnimator != nullptr)
    {
        m_pAnimator->updateBoneTransform(currentTime, deltaTime);
        m_boneTransformTex.bindData(m_pAnimator->finalTransformBuffer());
    }
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

        m_boneTransformTex.bind(8);
        m_mainShader->setUniform1i("boneTransform", 8);

        m_IBOList[i].bind();
        glDrawElements(GL_TRIANGLES, m_IBOList[i].getCount(), GL_UNSIGNED_INT, 0);
    }
}
PmxModelRenderer::PmxModelRenderer(pmx::Model *pModel, Shader *pShader, Shader *pDepthShader, PmxBoneAnimator *pAnimator)
    : ObjectBase(pShader, pDepthShader), m_pModel(pModel), m_pAnimator(pAnimator), currentTime(0.0f)
{
    assert(m_pModel != nullptr);

    m_VAO.create();
    m_VAO.bind();

    unsigned int stride = 17;

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

        switch (vl[i].deformMethod)
        {
        case 0:
            vertices[j + 8] = 1;
            vertices[j + 9] = vl[i].bindedBone[0];
            break;
        case 1:
        case 3:
            vertices[j + 8] = 2;
            vertices[j + 9] = vl[i].bindedBone[0];
            vertices[j + 10] = vl[i].bindedBone[1];
            vertices[j + 13] = vl[i].weights[0];
            vertices[j + 14] = vl[i].weights[1];
            break;
        case 2:
            vertices[j + 8] = 4;
            for (uint8_t k = 0; k < 4; ++k)
            {
                vertices[j + 9 + k] = vl[i].bindedBone[k];
                vertices[j + 13 + k] = vl[i].weights[k];
            }
            break;
        }
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
    layout.push(GL_FLOAT, 1);
    layout.push(GL_FLOAT, 4);
    layout.push(GL_FLOAT, 4);
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

    // animator
    m_boneTransformTex.createFloatBuffer(4, m_pModel->bones.size());
}

void PmxModelRenderer::onImGuiRender()
{
}