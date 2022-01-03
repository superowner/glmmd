#include <engine/Scene.h>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>

Scene::Scene() {}

void Scene::init(int width, int height, int shadowMapWidth, int shadowMapHeight)
{
    m_width = width;
    m_height = height;
    m_shadowMapWidth = shadowMapWidth;
    m_shadowMapHeight = shadowMapHeight;
    m_shadowMap.createDepthBuffer(shadowMapWidth, shadowMapHeight);
    m_camera.aspect = (float)width / (float)height;
    m_mainLight.dir = glm::vec3(-0.5f, -1.0f, 0.5f);
    m_mainLight.setDiffuse(0.8f, 0.8f, 0.8f);
    m_mainLight.setAmbient(0.3f, 0.3f, 0.3f);
}

void Scene::handleEvent(float deltaTime)
{
    while (!eventQueue.empty())
    {
        auto event = eventQueue.front();
        if (event.type == EventType::KEY_PRESSED)
        {
            int key = event.keyboardEvent.keyId;
            float vel = 15.f;
            switch (key)
            {
            case GLFW_KEY_W:
                m_camera.translate(-deltaTime * m_camera.dir * vel);
                break;
            case GLFW_KEY_S:
                m_camera.translate(deltaTime * m_camera.dir * vel);
                break;
            case GLFW_KEY_A:
                m_camera.translate(-deltaTime * m_camera.right * vel);
                break;
            case GLFW_KEY_D:
                m_camera.translate(deltaTime * m_camera.right * vel);
                break;
            default:
                break;
            }
        }
        eventQueue.pop();
    }
}

void Scene::onUpdate(float deltaTime)
{
    handleEvent(deltaTime);
    for (auto &pObj : m_objectList)
        pObj->onUpdate(deltaTime);
}

void Scene::onRenderShadowMap()
{
    float n = 1.0f, f = 8.0f;
    Camera lightCamera(0.0f, n, f, -f * 0.5f * m_mainLight.dir, glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 lightProjection = lightCamera.getOrthoProjMatrix(-10.0f, 10.0f, -10.0f, 10.0f);
    glm::mat4 lightView = lightCamera.getViewMatrix();
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    for (auto &pObj : m_objectList)
    {
        if (pObj->depthShader() != nullptr)
        {
            pObj->depthShader()->use();
            pObj->depthShader()->setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);
        }
    }
}

void Scene::onRender()
{
    glViewport(0, 0, m_width, m_height);
    for (auto &pObj : m_objectList)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = m_camera.getViewMatrix();
        glm::mat4 projection = m_camera.getProjMatrix();
        pObj->mainShader()->use();

        pObj->mainShader()->setUniformMatrix4fv("model", 1, GL_FALSE, model);
        pObj->mainShader()->setUniformMatrix4fv("view", 1, GL_FALSE, view);
        pObj->mainShader()->setUniformMatrix4fv("projection", 1, GL_FALSE, projection);

        pObj->mainShader()->setUniform3fv("viewPos", 1, m_camera.pos);

        pObj->mainShader()->setUniform3fv("mainLight.dir", 1, glm::normalize(m_mainLight.dir));
        pObj->mainShader()->setUniform3fv("mainLight.diffuseLight", 1, glm::vec3(m_mainLight.diffuse[0], m_mainLight.diffuse[1], m_mainLight.diffuse[2]));
        pObj->mainShader()->setUniform3fv("mainLight.ambientLight", 1, glm::vec3(m_mainLight.ambient[0], m_mainLight.ambient[1], m_mainLight.ambient[2]));

        pObj->onRender();
    }
}

void Scene::onImGuiRender()
{
    ImGui::SliderFloat("FOV", &m_camera.yfov, 0.1f, 120.0f);
    ImGui::SliderFloat("Main Light Dir.x", &m_mainLight.dir.x, -1.0f, 1.0f);
    ImGui::SliderFloat("Main Light Dir.y", &m_mainLight.dir.y, -1.0f, 1.0f);
    ImGui::SliderFloat("Main Light Dir.z", &m_mainLight.dir.z, -1.0f, 1.0f);
    ImGui::ColorEdit3("Main Light Diffuse", m_mainLight.diffuse);
    ImGui::ColorEdit3("Main Light Ambient", m_mainLight.ambient);
}

void Scene::addObject(ObjectBase *pObj)
{
    assert(pObj != nullptr);
    m_objectList.push_back(pObj);
}