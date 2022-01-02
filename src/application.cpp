#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <opengl/common.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <engine/PmxModelRenderer.h>
#include <engine/Scene.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::string projRootAbs("D:/Dev/cpp/glmmd/");

GLFWwindow *initWindow();

int main(int argc, char *argv[])
{
    pmx::Model model;
    model.loadFromFile(projRootAbs + "res/models/HakureiReimu0.958_Sheep3D/reimu_Sheep3D_0.958.pmx");

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

        Shader shader(projRootAbs + "res/shaders/basic_vert.shader", projRootAbs + "res/shaders/basic_frag.shader");
        PmxModelRenderer renderer(&model);
        Scene mainScene;
        mainScene.addObject(renderer);
        mainScene.addShader(shader);
        glEnable(GL_DEPTH_TEST);

        while (!glfwWindowShouldClose(window))
        {
            processInput(window);

            glClearColor(0.2f, 0.3f, 0.8f, 0.8f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                mainScene.onUpdate(0.0f);
                mainScene.onRender();
                ImGui::Begin("Control Panel");
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
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

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
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
