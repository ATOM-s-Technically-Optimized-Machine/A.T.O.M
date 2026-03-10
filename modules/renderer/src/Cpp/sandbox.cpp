#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <renderer/glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "core/ECS/ECS.hpp"
#include "core/ECS/Includes.hpp"
#include "renderer/GLFWInputProvider.hpp"
#include "renderer/Input.hpp"
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    ecs.addSystem<RenderSystem>(
        "modules/renderer/src/shaders/default.vert",
        "modules/renderer/src/shaders/default.frag",
        window
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

    GLFWInputProvider provider = GLFWInputProvider(window);
    InputState inputs;
    float last_time = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        
        float time = (float)glfwGetTime();
        pyramid_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));
        pyramid2_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));
        ecs.Update();
        
        provider.fetch(inputs);

        if (inputs.isPressed(Key::Escape))
            break;

        float now = glfwGetTime();
        float dt = now - last_time;
        last_time = now;
            
        glm::vec3 camera_forward = CameraComponent::getForward(cam_data);
        glm::vec3 right = glm::normalize((glm::cross(camera_forward, glm::vec3(0.0f, 1.0f, 0.0f))));
        glm::vec3 up = glm::normalize((glm::cross(right, camera_forward)));
        
        float speed = 3.0f * dt;

        if (inputs.isPressed(Key::W))           cam_data.position += camera_forward * speed;
        if (inputs.isPressed(Key::S))           cam_data.position -= camera_forward * speed;
        if (inputs.isPressed(Key::A))           cam_data.position -= right          * speed;
        if (inputs.isPressed(Key::D))           cam_data.position += right          * speed;
        if (inputs.isPressed(Key::Space))       cam_data.position += up             * speed;
        if (inputs.isPressed(Key::LShift))      cam_data.position -= up             * speed;

        float sensitivity = 0.1f;
        cam_data.yaw += inputs.mouseDx * sensitivity;
        cam_data.pitch -= inputs.mouseDy * sensitivity;
        cam_data.pitch = glm::clamp(cam_data.pitch, -89.0f, 89.0f);

        iter++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
