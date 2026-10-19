#ifndef RENDER_SYSTEM_HPP_
    #define RENDER_SYSTEM_HPP_

    #include "core/ECS/ECS.hpp"
    #include "core/ECS/Includes.hpp"
    #include "core/ECS/System.hpp"
    #include "core/DefaultComponents.hpp"
    #include "resources/ResourceTypes.hpp"
    #include <GLFW/glfw3.h>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/glm.hpp>
    #include <unordered_map>

class RenderSystem : public ECS::ISystem {
    public:
        RenderSystem(const std::string &vertShaderPath, const std::string &fragShaderPath, GLFWwindow *window, const resources::ImageData &defaultTexture);
        ~RenderSystem();

        unsigned int getShader(MeshHandleComponent::AttributesMask attrib);
        void setTargetFBO(unsigned int fbo, int fbo_w, int fbo_h);

        void Update(ECS::ECS &ecs, uint32_t ms) override;
    private:
        std::string m_vert_path;
        std::string m_frag_path;
        std::unordered_map<int, unsigned int> m_shader_cache;
        GLFWwindow *m_window;
        unsigned int m_default_texture;
        unsigned int m_fbo = 0;
        int m_fbo_w;
        int m_fbo_h;
};

#endif /* !RENDER_SYSTEM_HPP_ */
