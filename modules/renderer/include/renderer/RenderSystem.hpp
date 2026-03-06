#ifndef RENDER_SYSTEM_HPP_
    #define RENDER_SYSTEM_HPP_

    #include <GLFW/glfw3.h>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/glm.hpp>
    #include "core/ECS/ECS.hpp"
#include "core/ECS/Includes.hpp"
    #include "core/ECS/System.hpp"

struct MeshHandleComponent {
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

    static MeshHandleComponent Create(
        std::vector<float> &vertices,
        std::vector<unsigned int> &indices
    );
};

struct TransformComponent {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale = glm::vec3(1.0f);
};

using CameraComponent = glm::mat4;

class RenderSystem : public ECS::ISystem {
    public:
        RenderSystem(unsigned int shaderProgram, GLFWwindow *window, glm::mat4 view, glm::mat4 projection);
        ~RenderSystem();

        void Update(ECS::ECS &ecs, ECS::SystemID id, uint32_t ms) override;
    private:
        unsigned int m_shader_program = 0;
        GLFWwindow *m_window;
        glm::mat4 m_view;
        glm::mat4 m_projection;
};

#endif /* !RENDER_SYSTEM_HPP_ */
