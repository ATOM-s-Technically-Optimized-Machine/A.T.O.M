#include <renderer/glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/ECS/Includes.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "inputs/ECSIntegration.hpp"
#include "inputs/InputState.hpp"
#include "inputs/KeyCodes.hpp"
#include "renderer/RenderSystem.hpp"
#include "renderer/Renderer.hpp"
#include "resources/ResourceLoaders.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/ResourceTypes.hpp"
#include "tools/Editor.hpp"
#include "tools/window/AssetTreeWindow.hpp"
#include <iostream>

using namespace editor;

int main()
{
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "ATOM Editor", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int fbo, fbo_tex, fbo_depth;
    int fbo_w = 1280, fbo_h = 720;
    genFBO(fbo_w, fbo_h, fbo, fbo_tex, fbo_depth);

    resources::ResourceManager manager("/home/guilec06/delivery/EIP/ATOM/main");
    resources::registerDefaultLoaders(manager);
    resources::ImageData default_tex = manager.get<resources::ImageData>(
        manager.load<resources::ImageData>("assets/default.png")
    );

    ECS::ECS ecs;
    ecs.registerComponent<TransformComponent>();
    ecs.registerComponent<MeshHandleComponent>();
    ecs.registerComponent<Atom::Input::CameraComponent>();

    auto &render_sys = ecs.addSystem<RenderSystem>(
        "modules/renderer/src/shaders/default.vert",
        "modules/renderer/src/shaders/default.frag",
        window,
        default_tex
    );
    render_sys.setTargetFBO(fbo, fbo_w, fbo_h);

    auto &input_sys = ecs.addSystem<Atom::Input::InputECSSystem>();
    input_sys.bindGLFW(window);

    auto &cam_sys = ecs.addSystem<Atom::Input::CameraECSSystem>(
        input_sys.inputSystem()
    );

    ecs.systemSetPriority<Atom::Input::InputECSSystem>(0);
    ecs.systemSetPriority<Atom::Input::CameraECSSystem>(1);
    ecs.systemSetPriority<RenderSystem>(3);

    ECS::EntityID cam_entity = ecs.entityCreate();
    auto &cam = ecs.entityAddComponent<Atom::Input::CameraComponent>(cam_entity);
    cam.camera.setPerspective(70.0f, (float)fbo_w / fbo_h, 0.1f, 500.0f);
    cam.camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    cam.fpsBindings.lookButton = Atom::Input::MouseButton::Right;

    Editor editor(window, ecs, render_sys, manager);
    editor.setAssetPath("assets/");

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

    editor.addWindow<AssetTreeWindow>("assets/");

    float prev_time = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float time = (float)glfwGetTime();
        float dt = time - prev_time;
        prev_time = time;
        ecs.Update(static_cast<uint32_t>(dt * 1000));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        editor.render();

        ImGui::Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        Atom::Input::InputState state = input_sys.inputSystem()->state();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
