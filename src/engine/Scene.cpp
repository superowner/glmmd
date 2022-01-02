#include <engine/Scene.h>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
Scene::Scene()
    : m_camera()
{
}

void Scene::onUpdate(float deltaTime)
{
    while (!eventQueue.empty())
    {
        auto event = eventQueue.front();
        if (event.type == EventType::WINDOW_RESIZE)
        {
            m_camera.aspect = (float)event.windowEvent.width / (float)event.windowEvent.height;
        }
        else if (event.type == EventType::KEY_PRESSED)
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

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = m_camera.getViewMatrix();
    glm::mat4 projection = m_camera.getProjMatrix();

    m_shaderList[0]->use();
    unsigned int modelLoc = glGetUniformLocation(m_shaderList[0]->getId(), "model");
    unsigned int viewLoc = glGetUniformLocation(m_shaderList[0]->getId(), "view");
    unsigned int projLoc = glGetUniformLocation(m_shaderList[0]->getId(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
}

void Scene::onRender()
{

    for (auto &pObj : m_objectList)
        pObj->onRender();
}

void Scene::onImGuiRender()
{
    ImGui::SliderFloat("FOV", &m_camera.yfov, 0.1f, 120.0f);
}

void Scene::addShader(Shader &shader)
{
    m_shaderList.push_back(&shader);
}

void Scene::addObject(ObjectBase &obj)
{
    m_objectList.push_back(&obj);
}