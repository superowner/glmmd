#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <opengl/common.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <pmx/PmxModelRenderer.h>

#include <engine/OffscreenRenderer.h>
#include <engine/Scene.h>

#include <utils/GlobalConfig.h>

void processInput(GLFWwindow *window);

const std::string projRootDir("../");

Scene mainScene;

GLFWwindow *initWindow();

GlobalConfig Cfg(projRootDir + "res/GlobalConfig.json");

int main(int argc, char *argv[])
{
    GLFWwindow *window = initWindow();
    if (window == nullptr)
        return -1;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        mainScene.init(Cfg.ScreenWidth, Cfg.ScreenHeight, Cfg.ShadowMapWidth, Cfg.ShadowMapHeight,
                       Cfg.LightCamWidth, Cfg.LightCamHeight, Cfg.LightCamNear, Cfg.LightCamFar);

        pmx::Model model;
        model.loadFromFile(projRootDir + "res/models/HakureiReimu_v1.0/HakureiReimu.pmx");

        Shader shader(projRootDir + "res/shaders/mmd_style_vert.shader",
                      projRootDir + "res/shaders/mmd_style_frag.shader");
        Shader depthShader(projRootDir + "res/shaders/depth_vert.shader",
                           projRootDir + "res/shaders/depth_frag.shader");
        PmxModelRenderer renderer(&model, &shader, &depthShader);
        mainScene.addObject(&renderer);

        pmx::Model plane;
        plane.loadFromFile(projRootDir + "res/models/Plane.pmx");
        Shader planeShader(projRootDir + "res/shaders/mmd_style_vert.shader",
                           projRootDir + "res/shaders/mmd_style_frag.shader");
        Shader planeDepthShader(projRootDir + "res/shaders/depth_vert.shader",
                                projRootDir + "res/shaders/depth_frag.shader");
        PmxModelRenderer planeRenderer(&plane, &planeShader, &planeDepthShader);
        mainScene.addObject(&planeRenderer);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_FRAMEBUFFER_SRGB);

        Shader screenShader(projRootDir + "res/shaders/screen_vert.shader",
                            projRootDir + "res/shaders/screen_frag.shader");
        OffscreenRenderer offscreenRenderer(Cfg.ScreenWidth, Cfg.ScreenHeight, Cfg.AASamples, &screenShader);

        while (!glfwWindowShouldClose(window))
        {
            processInput(window);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                mainScene.onUpdate(1.0f / ImGui::GetIO().Framerate);
                mainScene.onRenderShadowMap();

                offscreenRenderer.begin();
                offscreenRenderer.onImGui();
                mainScene.onRender();
                offscreenRenderer.end();

                ImGui::Begin("Scene Control Panel");
                ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
                mainScene.onImGuiRender();
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();

    return 0;
}

GLFWwindow *initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(Cfg.ScreenWidth, Cfg.ScreenHeight, "GLMMD", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainScene.eventQueue.pushKeyboardPress(GLFW_KEY_W);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainScene.eventQueue.pushKeyboardPress(GLFW_KEY_S);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainScene.eventQueue.pushKeyboardPress(GLFW_KEY_A);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainScene.eventQueue.pushKeyboardPress(GLFW_KEY_D);
}

