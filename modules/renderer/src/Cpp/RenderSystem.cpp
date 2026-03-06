#include "renderer/glad/glad.h"
#include "renderer/RenderSystem.hpp"
#include "core/ECS/Includes.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

MeshHandleComponent MeshHandleComponent::Create(std::vector<float> &vertices, std::vector<unsigned int> &indices)
{
    MeshHandleComponent meshHandle;

    glGenVertexArrays(1, &meshHandle.VAO);
    glGenBuffers(1, &meshHandle.VBO);
    glGenBuffers(1, &meshHandle.EBO);

    glBindVertexArray(meshHandle.VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, meshHandle.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshHandle.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    meshHandle.indexCount = indices.size();

    return meshHandle;
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
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glad_glBindVertexArray(0);
    }

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
