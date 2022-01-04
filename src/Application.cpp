#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <opengl/common.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <pmx/PmxModelRenderer.h>
#include <pmx/VmdData.h>
#include <pmx/PmxBoneAnimator.h>

#include <engine/Scene.h>
#include <engine/OffscreenRenderer.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1440;
const unsigned int SCR_HEIGHT = 960;

const int SHADOW_MAP_WIDTH = 2048;
const int SHADOW_MAP_HEIGHT = 2048;

const std::string projRootDir("../");

Scene mainScene;

GLFWwindow *initWindow();

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
    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::StyleColorsDark();

        mainScene.init(SCR_WIDTH, SCR_HEIGHT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

        VmdData motion;
        motion.loadFromFile(projRootDir + "res/motions/ochame_kinou_left.vmd");

        pmx::Model model;
        model.loadFromFile(projRootDir + "res/models/DIYUSI/DIYUSI.pmx");
        // model.loadFromFile(projRootDir + "res/models/alice_alteanative_160907a/AliceMargatroid.pmx");
        PmxBoneAnimator animator(model, motion);

        Shader shader(projRootDir + "res/shaders/mmd_style_vert.shader", projRootDir + "res/shaders/mmd_style_frag.shader");
        Shader depthShader(projRootDir + "res/shaders/depth_vert.shader", projRootDir + "res/shaders/depth_frag.shader");
        PmxModelRenderer renderer(&model, &shader, &depthShader, &animator);
        mainScene.addObject(&renderer);

        pmx::Model plane;
        plane.loadFromFile(projRootDir + "res/models/Plane.pmx");
        Shader planeShader(projRootDir + "res/shaders/mmd_style_vert.shader", projRootDir + "res/shaders/mmd_style_frag.shader");
        PmxModelRenderer planeRenderer(&plane, &shader, &depthShader, nullptr);
        mainScene.addObject(&planeRenderer);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_FRAMEBUFFER_SRGB);
        OffscreenRenderer offscreenRenderer(SCR_WIDTH, SCR_HEIGHT, projRootDir + "res/shaders/screen_vert.shader", projRootDir + "res/shaders/screen_frag.shader");

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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLMMD", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
