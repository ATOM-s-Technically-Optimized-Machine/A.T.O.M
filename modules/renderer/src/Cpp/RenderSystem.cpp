#include "renderer/tiny_obj_loader.hpp"
#include "renderer/glad/glad.h"
#include "renderer/RenderSystem.hpp"
#include "core/ECS/Includes.hpp"
#include <GLFW/glfw3.h>
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

RenderSystem::RenderSystem(unsigned int shaderProgram, GLFWwindow *window, glm::mat4 view, glm::mat4 projection)
: m_shader_program(shaderProgram), m_window(window), m_view(view), m_projection(projection)
{
}

RenderSystem::~RenderSystem()
{
    glDeleteProgram(m_shader_program);
}

void RenderSystem::Update(ECS::ECS &ecs, ECS::SystemID id, uint32_t ms)
{
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(m_shader_program);
    glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(glGetUniformLocation(m_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));

    auto entities = ecs.getEntitiesByComponentsAllOf<MeshHandleComponent, TransformComponent>();

    for (auto e : entities) {
        auto &mesh = ecs.entityGetComponent<MeshHandleComponent>(e);
        auto &transform = ecs.entityGetComponent<TransformComponent>(e);
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        glUniformMatrix4fv(glad_glGetUniformLocation(m_shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glad_glBindVertexArray(mesh.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.texture);
        glUniform1i(glad_glGetUniformLocation(m_shader_program, "uTexture"), 0);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glad_glBindVertexArray(0);
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
