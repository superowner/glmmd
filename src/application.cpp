#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <pmx/PmxModel.h>

#include <utils/CodeConverter.h>

#include <opengl/common.h>
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::string projRootAbs("D:/Dev/cpp/glmmd/");

int main(int argc, char *argv[])
{
    pmx::Model model;
    model.loadFromFile(projRootAbs + "res/models/HakureiReimu0.958_Sheep3D/reimu_Sheep3D_0.958.pmx");
    std::cout << convertToNativeEncoding(model.info.modelNameEN, EncodingType::UTF16_LE) << std::endl;
    std::cout << model.vertices.size() << '\n';
    std::cout << model.indices.size() << '\n';

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader(projRootAbs + "res/shaders/basic_vert.shader", projRootAbs + "res/shaders/basic_frag.shader");

    VertexArrayObject VAO;
    VAO.create();
    VAO.bind();

    float *vertices = new float[model.vertices.size() * 3];

    for (size_t i = 0; i < model.vertices.size(); ++i)
    {
        vertices[3 * i + 0] = model.vertices[i].position.x * 0.1;
        vertices[3 * i + 1] = model.vertices[i].position.y * 0.1;
        vertices[3 * i + 2] = model.vertices[i].position.z * 0.1;
    }

    VertexBufferObject VBO;
    VBO.create(vertices, sizeof(float) * model.vertices.size() * 3);

    IndexBufferObject IBO;
    IBO.create(&model.indices[0], sizeof(unsigned int) * model.indices.size());

    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    VAO.addBuffer(VBO, layout);
    VBO.unbind();
    VAO.unbind();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        VAO.bind();
        glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    VAO.destroy();
    VBO.destroy();
    IBO.destroy();
    shader.destroy();

    glfwTerminate();

    return 0;
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
