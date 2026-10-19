#ifndef EDITOR_HPP_
    #define EDITOR_HPP_

#include "core/ECS/ECS.hpp"
#include "renderer/RenderSystem.hpp"
#include "resources/ResourceManager.hpp"
#include "tools/window/IWindow.hpp"
#include <concepts>
#include <filesystem>


struct GLFWwindow;
namespace editor {

void genFBO(int width, int height, unsigned int &out_fbo, unsigned int &out_tex, unsigned int &out_depth);

class Editor {
    public:
        Editor(GLFWwindow *window, ECS::ECS &ecs, RenderSystem &render, resources::ResourceManager &manager);
        ~Editor();

        void render();
        void renderDirTree(const std::filesystem::path &path);

        void setAssetPath(const std::filesystem::path &path);
        void reloadAssets();

        template<WindowClass T, typename... Args>
            requires std::constructible_from<T, editor::Editor &, Args...>
        T &addWindow(Args... args)
        {
            auto ptr = std::make_unique<T>(*this, std::forward<Args>(args)...);
            T &ref = *ptr;
            m_sub_windows.push_back(std::move(ptr));
            return ref;
        }

    protected:
    private:
        ECS::ECS &m_ecs;
        RenderSystem &m_render_sys;
        std::filesystem::path m_assets_path;
        bool m_assets_should_reload;
        unsigned int m_fbo = 0;
        unsigned int m_fbo_tex = 0;
        unsigned int m_fbo_depth;
        int m_fbo_w = 0;
        int m_fbo_h = 0;
        GLFWwindow *m_window;
        std::vector<std::unique_ptr<IWindow>> m_sub_windows;
};

}

#endif /* !EDITOR_HPP_ */
