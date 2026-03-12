#ifndef RENDER_SYSTEM_HPP_
    #define RENDER_SYSTEM_HPP_

    #include "core/ECS/ECS.hpp"
    #include "core/ECS/Includes.hpp"
    #include "core/ECS/System.hpp"
    #include "resources/ResourceTypes.hpp"
    #include <GLFW/glfw3.h>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/glm.hpp>
    #include <unordered_map>

struct MeshHandleComponent {
    using AttributesMask = unsigned char;

    enum Attributes {
        NONE =           0,
        HAS_UV =         1u << 1,
        HAS_NORMALS =    1u << 2,
        HAS_COLORS =     1u << 3,
    };

    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    unsigned int texture;

    AttributesMask attributes;

    static MeshHandleComponent Create(
        const std::vector<float> &vertices,
        const std::vector<unsigned int> &indices,
        AttributesMask attrib = Attributes::NONE
    );

    [[deprecated("Use resources::loadMesh instead")]]
    static MeshHandleComponent loadMesh(const std::string &objPath);
    static MeshHandleComponent fromMeshData(const resources::MeshData &data);
};

struct TransformComponent {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale = glm::vec3(1.0f);
};

struct CameraComponent {
    glm::vec3 position = glm::vec3(0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;
    bool enabled = true;

    struct {
        float x = 0.0f;
        float y = 0.0f;
        float width = 1.0f;
        float height = 1.0f;
    } ViewPort;

    static glm::mat4 getView(const CameraComponent &cam);
    static glm::mat4 getProjection(const CameraComponent &cam, float aspectRatio);
    static glm::vec3 getForward(const CameraComponent &cam);
};

class RenderSystem : public ECS::ISystem {
    public:
        RenderSystem(const std::string &vertShaderPath, const std::string &fragShaderPath, GLFWwindow *window, const resources::ImageData &defaultTexture);
        ~RenderSystem();

        unsigned int getShader(MeshHandleComponent::AttributesMask attrib);

        void Update(ECS::ECS &ecs, ECS::SystemID id, uint32_t ms) override;
    private:
        std::string m_vert_path;
        std::string m_frag_path;
        std::unordered_map<int, unsigned int> m_shader_cache;
        GLFWwindow *m_window;
        unsigned int default_texture;
};

#endif /* !RENDER_SYSTEM_HPP_ */
