#ifndef EDITOR_HPP_
    #define EDITOR_HPP_

#include "core/ECS/ECS.hpp"
#include "resources/ResourceManager.hpp"
#include "tools/panels/IPanel.hpp"
#include <memory>

struct GLFWwindow;
namespace editor {

void genFBO(int width, int height, unsigned int &out_fbo, unsigned int &out_tex, unsigned int &out_depth);

struct ViewEntry {
    std::size_t id;
    bool is_open = true;
    std::string name;
    std::weak_ptr<IPanel> panel;
};

struct PanelRegistryEntry {
    std::string name;
    std::string lowercase_name;
    std::shared_ptr<IPanel> panel;
};

class Editor {
    public:
        Editor(GLFWwindow *window, ECS::ECS &ecs, resources::ResourceManager &manager);
        ~Editor();

        void render();

        void commandPalette();

        int spawnView(const std::string &name);
        void killView(int id);

        template<PanelClass T, typename... Args>
            requires std::constructible_from<T, editor::Editor &, Args...>
        T &registerPanel(std::string name, Args&&... args)
        {
            const auto &exists = std::find_if(m_registry.begin(), m_registry.end(), [&name](PanelRegistryEntry &entry) { return entry.name == name; });
            // Skips registry if entry already exists
            if (exists != m_registry.end())
                return static_cast<T&>(*exists->panel);

            std::shared_ptr<IPanel> ptr = std::make_shared<T>(*this, std::forward<Args>(args)...);
            auto &ref = *ptr;

            std::string l_name(name);
            std::transform(
                l_name.begin(),
                l_name.end(),
                l_name.begin(),
                [](unsigned char c) { return std::tolower(c); }
            );

            // Register panel in the registry
            m_registry.push_back({
                .name = std::move(name),
                .lowercase_name = std::move(l_name),
                .panel = std::move(ptr)
            });

            return static_cast<T&>(ref);
        }

        std::shared_ptr<IPanel> getPanel(const std::string &name);

    protected:
    private:
        ECS::ECS &m_ecs;
        GLFWwindow *m_window;
        std::vector<PanelRegistryEntry> m_registry;
        std::unordered_map<std::size_t, ViewEntry> m_views;
        bool m_palette_open = false;
        int m_palette_selected = 0;
        char m_palette_buff[128] = {};
        int m_ids = 1;
};

}

#endif /* !EDITOR_HPP_ */
