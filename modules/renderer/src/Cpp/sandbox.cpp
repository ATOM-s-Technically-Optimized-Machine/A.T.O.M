#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
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
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f
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

    unsigned int obama = renderer::loadTexture("modules/renderer/obama.png");
    MeshHandleComponent obama_mesh = MeshHandleComponent::loadMesh("modules/renderer/obamid.mesh");

    ECS::ECS ecs = ECS::ECS();

    ecs.registerComponent<MeshHandleComponent>();
    ecs.registerComponent<TransformComponent>();
    ecs.registerComponent<CameraComponent>();

    ECS::EntityID camera = ecs.entityCreate();

    auto &cam_data = ecs.entityAddComponent<CameraComponent>(camera);
    cam_data.fov = 70.0f;
    cam_data.position = glm::vec3(0.0f, 1.0f, 5.0f);
    cam_data.ViewPort.height = 0.5f;
    cam_data.ViewPort.width = 0.5f;

    ECS::EntityID camera2 = ecs.entityCreate();

    auto &cam2_data = ecs.entityAddComponent<CameraComponent>(camera2);
    cam2_data.fov = 70.0f;
    cam2_data.position = glm::vec3(3.0f, 1.0f, 3.0f);
    cam2_data.yaw = -135.0f;
    cam2_data.ViewPort.y = 0.5f;
    cam2_data.ViewPort.height = 0.5f;
    cam2_data.ViewPort.width = 0.5f;

    ECS::EntityID camera3 = ecs.entityCreate();

    auto &cam3_data = ecs.entityAddComponent<CameraComponent>(camera3);
    cam3_data.fov = 70.0f;
    cam3_data.position = glm::vec3(-2.0f, 1.0f, -2.0f);
    cam3_data.yaw = 45.0f;
    cam3_data.ViewPort.x = 0.5f;
    cam3_data.ViewPort.height = 0.5f;
    cam3_data.ViewPort.width = 0.5f;

    ECS::EntityID camera4 = ecs.entityCreate();

    auto &cam4_data = ecs.entityAddComponent<CameraComponent>(camera4);
    cam4_data.fov = 70.0f;
    cam4_data.position = glm::vec3(-5.0f, 1.0f, 0.0f);
    cam4_data.yaw = 0.0f;
    cam4_data.ViewPort.y = 0.5f;
    cam4_data.ViewPort.x = 0.5f;
    cam4_data.ViewPort.height = 0.5f;
    cam4_data.ViewPort.width = 0.5f;

    ecs.addSystem<RenderSystem>(
        "modules/renderer/src/shaders/default.vert",
        "modules/renderer/src/shaders/default.frag",
        window,
        camera
    );

    ECS::EntityID pyramid = ecs.entityCreate();

    auto &pyramid_mesh = ecs.entityAddComponent<MeshHandleComponent>(pyramid);
    auto &pyramid_transform = ecs.entityAddComponent<TransformComponent>(pyramid);

    pyramid_mesh = obama_mesh;
    pyramid_mesh.texture = obama;

    ECS::EntityID pyramid2 = ecs.entityCreate();

    auto &pyramid2_mesh = ecs.entityAddComponent<MeshHandleComponent>(pyramid2);
    auto &pyramid2_transform = ecs.entityAddComponent<TransformComponent>(pyramid2);

    pyramid2_mesh = obama_mesh;
    pyramid2_mesh.texture = obama;

    pyramid2_transform.position.x = 1.5f;
    pyramid2_transform.scale = glm::vec3(0.5f);

    while (!glfwWindowShouldClose(window)) {
        ecs.Update();

        float time = (float)glfwGetTime();
        pyramid_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));
        pyramid2_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));

        cam_data.position.z = 3 + glm::sin(time);

        iter++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
