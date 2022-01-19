#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <opengl/common.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <pmx/PmxModelRenderer.h>

#include <engine/OffscreenRenderer.h>
#include <engine/Scene.h>

#include <utils/GlobalConfig.h>
#include <utils/Logger.h>

void processInput(GLFWwindow *window);

const std::string projRootDir("../");

Scene mainScene;

GLFWwindow *initWindow();
void initImGui(GLFWwindow *);

GlobalConfig Cfg(projRootDir + "res/GlobalConfig.json");

int main(int argc, char *argv[])
{
    GLFWwindow *window = initWindow();

    Logger::init();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        GLMMD_LOG_ERROR("Failed to initialize GLAD");
        return -1;
    }
    glfwSwapInterval(1);
    GLMMD_LOG_INFO("GL version: {}", glGetString(GL_VERSION));

    {
        initImGui(window);

        mainScene.init(Cfg.ScreenWidth, Cfg.ScreenHeight, Cfg.ShadowMapWidth, Cfg.ShadowMapHeight,
                       Cfg.LightCamWidth, Cfg.LightCamHeight, Cfg.LightCamNear, Cfg.LightCamFar);

        // Load Shaders

        Shader modelShader, modelDepthShader, planeShader, planeDepthShader, screenShader;
        try
        {
            modelShader.create(projRootDir + "res/shaders/mmd_style_vert.shader",
                               projRootDir + "res/shaders/mmd_style_frag.shader");
            modelDepthShader.create(projRootDir + "res/shaders/depth_vert.shader",
                                    projRootDir + "res/shaders/depth_frag.shader");

            planeShader.create(projRootDir + "res/shaders/mmd_style_vert.shader",
                               projRootDir + "res/shaders/mmd_style_frag.shader");
            planeDepthShader.create(projRootDir + "res/shaders/depth_vert.shader",
                                    projRootDir + "res/shaders/depth_frag.shader");

            screenShader.create(projRootDir + "res/shaders/screen_vert.shader",
                                projRootDir + "res/shaders/screen_frag.shader");
        }
        catch (const std::pair<ShaderErr, std::string> &err)
        {
            switch (err.first)
            {
            case FILE_LOAD_ERR:
                GLMMD_LOG_ERROR("Failed to load shader from file");
                break;
            case VERT_COMPILE_ERR:
                GLMMD_LOG_ERROR("Vertex shader compile error:\n{}", err.second);
                break;
            case FRAG_COMPILE_ERR:
                GLMMD_LOG_ERROR("Fragment shader compile error:\n{}", err.second);
                break;
            case LINKING_ERR:
                GLMMD_LOG_ERROR("Linking error:\n{}", err.second);
                break;
            }
            exit(-1);
        }

        // Load Models
        pmx::Model model;
        pmx::Model plane;

        try
        {
            model.loadFromFile(projRootDir + "res/models/HakureiReimu_v1.0/HakureiReimu.pmx");
        }
        catch (const std::runtime_error &err)
        {
            GLMMD_LOG_ERROR("Failed to load model from {}",
                            projRootDir + "res/models/HakureiReimu_v1.0/HakureiReimu.pmx");
        }

        try
        {
            plane.loadFromFile(projRootDir + "res/models/Plane.pmx");
        }
        catch (const std::runtime_error &err)
        {
            GLMMD_LOG_ERROR("Failed to load model from {}",
                            projRootDir + "res/models/HakureiReimu_v1.0/HakureiReimu.pmx");
        }
        PmxModelRenderer renderer(&model, &modelShader, &modelDepthShader);
        PmxModelRenderer planeRenderer(&plane, &planeShader, &planeDepthShader);
        // Create Scene
        mainScene.addObject(&renderer);
        mainScene.addObject(&planeRenderer);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_FRAMEBUFFER_SRGB);

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
        GLMMD_LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void initImGui(GLFWwindow *window)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
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
