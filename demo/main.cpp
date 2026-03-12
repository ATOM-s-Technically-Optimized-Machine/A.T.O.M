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
#include "resources/ResourceIncludes.hpp"
#include "resources/ResourceLoaders.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/ResourceTypes.hpp"

std::vector<float> vertices = {
    //  x      y      z      u     v
    // Front face
     0.0f,  0.5f,  0.0f,  0.5f, 1.0f,  // apex
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // front-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // front-left
    // Right face
     0.0f,  0.5f,  0.0f,  0.5f, 1.0f,  // apex
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // back-right
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // front-right
    // Back face
     0.0f,  0.5f,  0.0f,  0.5f, 1.0f,  // apex
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // back-left
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // back-right
    // Left face
     0.0f,  0.5f,  0.0f,  0.5f, 1.0f,  // apex
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // front-left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // back-left
    // Bottom face
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // front-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  // back-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // back-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // front-left
};

std::vector<unsigned int> indices = {
     0,  1,  2,  // Front
     3,  4,  5,  // Right
     6,  7,  8,  // Back
     9, 10, 11,  // Left
    12, 13, 14,  // Bottom tri 1
    12, 14, 15,  // Bottom tri 2
};

std::vector<float> cubeVertices = {
    // x      y      z     u     v
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    // Back face
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // Left face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // Right face
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    // Top face
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
};

std::vector<unsigned int> cubeIndices = {
     0,  1,  2,   0,  2,  3,   // Front
     4,  5,  6,   4,  6,  7,   // Back
     8,  9, 10,   8, 10, 11,   // Left
    12, 13, 14,  12, 14, 15,   // Right
    16, 17, 18,  16, 18, 19,   // Bottom
    20, 21, 22,  20, 22, 23,   // Top
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

    resources::ResourceManager manager("/home/guilec06/delivery/EIP/ATOM/main");
    resources::registerDefaultLoaders(manager);
    resources::ImageData resource_obama_texture = manager.get<resources::ImageData>(manager.load<resources::ImageData>("assets/obama.png"));
    resources::MeshData resource_obama_mesh = manager.get<resources::MeshData>(manager.load<resources::MeshData>("assets/obamid.mesh"));
    resources::ImageData default_tex = manager.get<resources::ImageData>(manager.load<resources::ImageData>("assets/default.png"));

    unsigned int obama = renderer::loadTextureFromImageData(resource_obama_texture);
    MeshHandleComponent obama_mesh = MeshHandleComponent::fromMeshData(resource_obama_mesh);
    
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
        window,
        default_tex
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

    ECS::EntityID cube = ecs.entityCreate();

    auto &cube_mesh = ecs.entityAddComponent<MeshHandleComponent>(cube);
    auto &cube_transform = ecs.entityAddComponent<TransformComponent>(cube);

    cube_mesh = MeshHandleComponent::Create(cubeVertices, cubeIndices, MeshHandleComponent::Attributes::HAS_UV);
    // cube_mesh.texture = obama;
    cube_transform.position = glm::vec3(2.0f, 1.0f, 3.0f);

    ECS::EntityID default_pyramid = ecs.entityCreate();

    auto &d_pyramid_mesh = ecs.entityAddComponent<MeshHandleComponent>(default_pyramid);
    auto &d_pyramid_transform = ecs.entityAddComponent<TransformComponent>(default_pyramid);

    d_pyramid_mesh = MeshHandleComponent::Create(vertices, indices, MeshHandleComponent::Attributes::HAS_UV);
    // cube_mesh.texture = obama;
    d_pyramid_transform.position = glm::vec3(-2.0f, 1.0f, 3.0f);

    GLFWInputProvider provider = GLFWInputProvider(window);
    InputState inputs;
    float last_time = 0.0f;
    bool allow_mouse = false;

    while (!glfwWindowShouldClose(window)) {
        
        float time = (float)glfwGetTime();
        pyramid_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));
        pyramid2_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));
        ecs.Update();
        
        provider.fetch(inputs);

        if (inputs.isPressed(Key::Escape)) {
            if (allow_mouse)
                break;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            allow_mouse = true;
        }
        if (inputs.isPressed(Key::Enter) && allow_mouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            allow_mouse = false;
        }

        float now = glfwGetTime();
        float dt = now - last_time;
        last_time = now;
            
        glm::vec3 camera_forward = CameraComponent::getForward(cam_data);
        glm::vec3 right = glm::normalize((glm::cross(camera_forward, glm::vec3(0.0f, 1.0f, 0.0f))));
        glm::vec3 up = glm::normalize((glm::cross(right, camera_forward)));
        
        float speed = 3.0f * dt;

        if (inputs.isDown(Key::W))           cam_data.position += camera_forward * speed;
        if (inputs.isDown(Key::S))           cam_data.position -= camera_forward * speed;
        if (inputs.isDown(Key::A))           cam_data.position -= right          * speed;
        if (inputs.isDown(Key::D))           cam_data.position += right          * speed;
        if (inputs.isDown(Key::Space))       cam_data.position += up             * speed;
        if (inputs.isDown(Key::LShift))      cam_data.position -= up             * speed;
        if (inputs.isDown(Key::PlusSign))    cam_data.fov      += 1                     ;
        if (inputs.isDown(Key::MinusSign))   cam_data.fov      -= 1                     ;


        float sensitivity = 0.1f;

        if (!allow_mouse) {
            cam_data.yaw += inputs.mouseDx * sensitivity;
            cam_data.pitch -= inputs.mouseDy * sensitivity;
            cam_data.pitch = glm::clamp(cam_data.pitch, -89.0f, 89.0f);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
