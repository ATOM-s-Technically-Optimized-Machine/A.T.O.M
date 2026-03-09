#include "renderer/Renderer.hpp"
#include "renderer/tiny_obj_loader.hpp"
#include "renderer/glad/glad.h"
#include "renderer/RenderSystem.hpp"
#include "core/ECS/Includes.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

MeshHandleComponent MeshHandleComponent::Create(
    std::vector<float> &vertices,
    std::vector<unsigned int> &indices,
    Attributes attrib
)
{
    MeshHandleComponent meshHandle;
    unsigned int vertex_size =
            3   // 3D space
        +   2 * !!(Attributes::UV_MAP & attrib)
        +   3 * !!(Attributes::COLORED & attrib)
    ;

    float cx = 0, cy = 0, cz = 0;
    for (int i = 0; i < vertices.size(); i += 5) {
        cx += vertices[i]; cy += vertices[i+1]; cz += vertices[i+2];
    }
    int count = vertices.size() / 5;
    cx /= count; cy /= count; cz /= count;

    for (int i = 0; i < vertices.size(); i += 5) {
        vertices[i] -= cx; vertices[i+1] -= cy; vertices[i+2] -= cz;
    }

    glGenVertexArrays(1, &meshHandle.VAO);
    glGenBuffers(1, &meshHandle.VBO);
    glGenBuffers(1, &meshHandle.EBO);

    glBindVertexArray(meshHandle.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, meshHandle.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    if (Attributes::COLORED & attrib) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    if (Attributes::UV_MAP & attrib) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (void*)((3 + 3 * (!!(Attributes::COLORED & attrib))) * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshHandle.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    meshHandle.indexCount = indices.size();
    meshHandle.attributes = attrib;

    return meshHandle;
}

MeshHandleComponent MeshHandleComponent::loadMesh(const std::string &objPath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str());

    if (!warn.empty()) std::cerr << "OBJ warn: " << warn << std::endl;
    if (!err.empty())  std::cerr << "OBJ err: "  << err  << std::endl;
    if (shapes.empty()) std::cerr << "OBJ: no shapes loaded from " << objPath << std::endl;

    std::vector<float> verticies;
    std::vector<unsigned int> indices;
    unsigned int idx = 0;

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            verticies.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            verticies.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            verticies.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            verticies.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
            verticies.push_back(1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);

            indices.push_back(idx++);
        }
    }

    return MeshHandleComponent::Create(verticies, indices, Attributes::UV_MAP);
}

glm::mat4 CameraComponent::getView(const CameraComponent &cam)
{
    glm::vec3 forward;

    forward.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    forward.y = sin(glm::radians(cam.pitch));
    forward.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
    forward = glm::normalize(forward);

    return glm::lookAt(cam.position, cam.position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 CameraComponent::getProjection(const CameraComponent &cam, float aspectRatio)
{
    return glm::perspective(glm::radians(cam.fov), aspectRatio, 0.1f, 100.0f);
}

RenderSystem::RenderSystem(
    const std::string &vertShaderPath,
    const std::string &fragShaderPath,
    GLFWwindow *window,
    ECS::EntityID cameraID
) :
    m_vert_path(vertShaderPath),
    m_frag_path(fragShaderPath),
    m_window(window),
    m_active_camera(cameraID)
{
}

RenderSystem::~RenderSystem()
{
    for (auto &shader : m_shader_cache) {
        glDeleteProgram(shader.second);
    }
}

unsigned int RenderSystem::getShader(MeshHandleComponent::Attributes attrib)
{
    if (m_shader_cache.count(attrib))
        return m_shader_cache[attrib];

    std::string defines = "";
    if (attrib & MeshHandleComponent::Attributes::UV_MAP) defines += "#define HAS_UV\n";
    if (attrib & MeshHandleComponent::Attributes::COLORED) defines += "#define HAS_COLOR\n";

    unsigned int prog = renderer::loadShaderProgram(m_vert_path, m_frag_path, defines);
    m_shader_cache[attrib] = prog;
    return prog;
}

void RenderSystem::Update(ECS::ECS &ecs, ECS::SystemID id, uint32_t ms)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    auto entities = ecs.getEntitiesByComponentsAllOf<MeshHandleComponent, TransformComponent>();

    auto &camera = ecs.entityGetComponent<CameraComponent>(m_active_camera);

    glm::mat4 view = camera.getView(camera);
    glm::mat4 projection = camera.getProjection(camera, 800.0f / 600.0f);

    for (auto e : entities) {
        auto &mesh = ecs.entityGetComponent<MeshHandleComponent>(e);
        auto &transform = ecs.entityGetComponent<TransformComponent>(e);
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        unsigned int shader_prog = this->getShader(mesh.attributes);

        glUseProgram(shader_prog);
        glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


        glUniformMatrix4fv(glad_glGetUniformLocation(shader_prog, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glad_glBindVertexArray(mesh.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.texture);
        glUniform1i(glad_glGetUniformLocation(shader_prog, "uTexture"), 0);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glad_glBindVertexArray(0);
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
