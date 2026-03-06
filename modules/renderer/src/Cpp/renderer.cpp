#include <glm/trigonometric.hpp>
#include <renderer/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "core/ECS/ECS.hpp"
#include "core/ECS/Includes.hpp"
#include "renderer/RenderSystem.hpp"
#include "renderer/Renderer.hpp"

std::vector<float> vertices = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f
};

std::vector<unsigned int> indices = {
    0, 1, 4,
    0, 2, 1,
    0, 3, 2,
    0, 4, 3,
    1, 2, 3,
    1, 3, 4
};

std::vector<float> cubeVertices = {
    -0.5, -0.5, 0.5,
    0.5, -0.5, 0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    -0.5, 0.5, 0.5,
    0.5, 0.5, 0.5,
    0.5, 0.5, -0.5,
    -0.5, 0.5, -0.5
};

std::vector<unsigned int> cubeIndices {
    0, 1, 5,
    0, 4, 5,
    0, 1, 2,
    0, 3, 2,
    0, 4, 7,
    0, 3, 7, 
    6, 2, 3,
    6, 7, 3,
    6, 5, 1,
    6, 2, 1,
    6, 7, 4,
    6, 5, 4
};

int main(void)
{
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "ATOM renderer", nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    int shaderProgram = renderer::loadShaderProgram(
        "modules/renderer/src/shaders/default.vert",
        "modules/renderer/src/shaders/default.frag"
    );

    int iter = 0;
    int mode = 0;

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 1.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f
    );

    ECS::ECS ecs = ECS::ECS();

    ecs.registerComponent<MeshHandleComponent>();
    ecs.registerComponent<TransformComponent>();

    ecs.addSystem<RenderSystem>(shaderProgram, window, view, projection);

    ECS::EntityID pyramid = ecs.entityCreate();

    auto &pyramid_mesh = ecs.entityAddComponent<MeshHandleComponent>(pyramid);
    auto &pyramid_transform = ecs.entityAddComponent<TransformComponent>(pyramid);

    pyramid_mesh = MeshHandleComponent::Create(vertices, indices);
    pyramid_transform.position.x -= 1;

    ECS::EntityID cube = ecs.entityCreate();

    auto &cube_mesh = ecs.entityAddComponent<MeshHandleComponent>(cube);
    auto &cube_transform = ecs.entityAddComponent<TransformComponent>(cube);

    cube_mesh = MeshHandleComponent::Create(cubeVertices, cubeIndices);
    cube_transform.position.x += 1;
    cube_transform.scale = glm::vec3(0.5f);

    while (!glfwWindowShouldClose(window)) {
        ecs.Update();

        float time = (float)glfwGetTime();
        cube_transform.rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0, 1, 1));
        pyramid_transform.rotation = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1, -1, 0));

        iter++;
    }

    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
