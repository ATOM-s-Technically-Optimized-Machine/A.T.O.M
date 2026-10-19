#include "inputs/InputState.hpp"
#include "inputs/KeyCodes.hpp"

#include <atomic>
#include <cstdlib>
#include <ctime>
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
#include <unordered_map>
#include "core/ECS/ECS.hpp"
#include "core/ECS/Includes.hpp"
#include "core/ECS/System.hpp"
#include "inputs/ECSIntegration.hpp"
#include "inputs/InputSystem.hpp"
#include "renderer/GLFWInputProvider.hpp"
#include "renderer/Input.hpp"
#include "renderer/RenderSystem.hpp"
#include "renderer/Renderer.hpp"
#include "resources/ResourceIncludes.hpp"
#include "resources/ResourceLoaders.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/ResourceTypes.hpp"

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


    // Create resource manager
    resources::ResourceManager manager(ATOM_ASSET_ROOT);
    resources::registerDefaultLoaders(manager);

    // Load static resources
    // resources::ImageData resource_obama_texture = manager.get<resources::ImageData>(manager.load<resources::ImageData>("assets/obama.png"));
    // resources::MeshData resource_obama_mesh = manager.get<resources::MeshData>(manager.load<resources::MeshData>("assets/obamid.mesh"));
    resources::ImageData default_tex = manager.get<resources::ImageData>(manager.load<resources::ImageData>("assets/default.png"));


    // Create OpenGL texture and mesh from resource manager data
    // unsigned int obama = renderer::loadTextureFromImageData(resource_obama_texture);
    // MeshHandleComponent obama_mesh = MeshHandleComponent::fromMeshData(resource_obama_mesh);

    // Create FBO for rendering in ports
    int fbo_w = 1280, fbo_h = 720;

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    unsigned int fbo_texture;
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_w, fbo_h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    unsigned int fbo_depth;
    glGenRenderbuffers(1, &fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbo_w, fbo_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FBO incomplete: 0x" << std::hex << status << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create ECS
    ECS::ECS ecs = ECS::ECS();

    // Register components
    ecs.registerComponent<MeshHandleComponent>();
    ecs.registerComponent<TransformComponent>();
    ecs.registerComponent<Atom::Input::CameraComponent>();

    // Create camera entity
    ECS::EntityID camera = ecs.entityCreate();

    auto &cam_data = ecs.entityAddComponent<Atom::Input::CameraComponent>(camera);
    cam_data.camera.setPerspective(70.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    cam_data.camera.setPosition(glm::vec3(0.0f, 1.0f, 5.0f));

    // Create systems
    ecs.addSystem<RenderSystem>(
        std::string(ATOM_ASSET_ROOT) + "/modules/renderer/src/shaders/default.vert",
        std::string(ATOM_ASSET_ROOT) + "/modules/renderer/src/shaders/default.frag",
        window,
        default_tex
    );

    auto &input_sys = ecs.addSystem<Atom::Input::InputECSSystem>();
    input_sys.bindGLFW(window);

    ecs.addSystem<Atom::Input::CameraECSSystem>(input_sys.inputSystem());

    // Obama prism
    // ECS::EntityID pyramid = ecs.entityCreate();

    // auto &pyramid_mesh = ecs.entityAddComponent<MeshHandleComponent>(pyramid);
    // auto &pyramid_transform = ecs.entityAddComponent<TransformComponent>(pyramid);

    // pyramid_mesh = obama_mesh;
    // pyramid_mesh.texture = obama;

    // Batch load and use objects meshes/textures
    resources::ResourceID texture_resource_id = manager.load<resources::ImageData>("assets/kenney_holiday-kit/Models/OBJ format/Textures/colormap.png");
    manager.scan<resources::MeshData>("assets/kenney_holiday-kit/Models/OBJ format", {".obj"});

    unsigned int texture_id = renderer::loadTextureFromImageData(manager.get<resources::ImageData>(texture_resource_id));

    std::srand(std::time(nullptr));
    auto &mesh_pool = manager.pool<resources::MeshData>();
    int mesh_count = mesh_pool.count();
    int half = 20;
    int obj_count = 200;

    std::unordered_map<int, MeshHandleComponent> gpu_meshes;
    for (int id = 1; id <= mesh_count; id++) {
        gpu_meshes[id] = MeshHandleComponent::fromMeshData(mesh_pool.get(id));
    }

    for (int i = 0; i < obj_count; i++) {
        int r = (std::rand() % mesh_count) + 1;

        auto &mesh = mesh_pool.get(r);
        ECS::EntityID e = ecs.entityCreate();
        
        auto &e_mesh = ecs.entityAddComponent<MeshHandleComponent>(e);
        auto &e_transform = ecs.entityAddComponent<TransformComponent>(e);
        e_mesh = gpu_meshes[r];
        e_mesh.texture = texture_id;
        e_transform.position = glm::vec3(
            static_cast<float>((std::rand() % (2 * half)) - half),
            0.0f,
            static_cast<float>((std::rand() % (2 * half)) - half)
        );

        e_transform.rotation = glm::quat(glm::vec3(
            0.0f,
            glm::radians(static_cast<float>(std::rand() % 360)),
            0.0f
        ));
    }

    float last_time = 0.0f;
    bool allow_mouse = false;
    float cam_fov = 70.0f;

    while (!glfwWindowShouldClose(window)) {

        float time = (float)glfwGetTime();
        float dt = time - last_time;
        ecs.Update(static_cast<uint32_t>(dt * 1000));
        last_time = time;
        
        // pyramid_transform.rotation = glm::angleAxis(time, glm::vec3(0.0f, 1.0f, 0.0f));

        const Atom::Input::InputState &state = input_sys.inputSystem()->state();

        if (state.isKeyJustReleased(Atom::Input::KeyCode::G)) {
            std::vector<ECS::EntityID> cameras;
            ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
            ECS::EntityID id = cameras[0];
            auto &cam_comp = ecs.entityGetComponent<Atom::Input::CameraComponent>(id);

            float distance = 2.0f;

            // calculer position

            glm::vec3 position = cam_comp.camera.position() + (cam_comp.camera.forward() * distance);

            // créer entitée
            ECS::EntityID entity = ecs.entityCreate();
            // assigner composant

            auto &transform = ecs.entityAddComponent<TransformComponent>(entity);
            auto &mesh = ecs.entityAddComponent<MeshHandleComponent>(entity);
            // récupérer mesh aléatoire

            int model_id = (std::rand() % gpu_meshes.size()) + 1;
            mesh = gpu_meshes[model_id];
            // définir la position

            transform.position = position;
            // définir la rotation (aléatoire)
            transform.rotation = glm::quat(glm::vec3(
                0.0f,
                glm::radians(static_cast<float>(std::rand() % 360)),
                0.0f
            ));
        }

        if (state.isKeyJustReleased(Atom::Input::KeyCode::Escape)) {
            if (allow_mouse)
                break;
            allow_mouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        if (allow_mouse && state.isKeyJustReleased(Atom::Input::KeyCode::Enter)) {
            allow_mouse = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
