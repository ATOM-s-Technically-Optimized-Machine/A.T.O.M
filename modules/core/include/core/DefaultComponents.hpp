#ifndef DEFAULTCOMPONENTS_HPP_
    #define DEFAULTCOMPONENTS_HPP_

#include "core/CoreUtils.hpp"
#include "core/ECS/Includes.hpp"
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>

namespace resources {
    struct MeshData;
};

// --------------------------------------------------------------------------------
// Components
// --------------------------------------------------------------------------------

struct HierarchyNode {
    ECS::EntityID parent = ECS::NULL_ENTITY;
    std::vector<ECS::EntityID> children;
};

using NameComponent = std::string;


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

    // Generates a matrix that translates local transforms into global space
    static glm::mat4 toWorld(const TransformComponent &t);
    // Generates a matrix that translates global transforms into t's local space
    static glm::mat4 toLocal(const TransformComponent &t);
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


// --------------------------------------------------------------------------------
// TAGS
// tags must be empty structs in order to be used as such in the ECS
// If you use a non-empty struct as a tag, it will not be space-optimized
// and addition logic may happen when trying to work with the data
// --------------------------------------------------------------------------------

struct SceneTag {};

#endif /* !DEFAULTCOMPONENTS_HPP_ */
