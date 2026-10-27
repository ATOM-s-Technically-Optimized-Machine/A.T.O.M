#ifndef SCENEVIEWERPANEL_HPP_
#define SCENEVIEWERPANEL_HPP_

#include "core/ECS/ECS.hpp"
#include "tools/panels/APanel.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct FBOContext {
    unsigned int fbo = 0;
    unsigned int tex = 0;
    unsigned int depth = 0;
    int w = 0;
    int h = 0;
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

class SceneViewerPanel : public APanel {
    public:
        SceneViewerPanel(editor::Editor &editor, GLFWwindow *window, ECS::ECS &ecs);
        ~SceneViewerPanel();

        void init();

        void render(int view_id) override;
        void contextMenu(int view_id) override;

        void onViewClose(int view_id) override;

        static constexpr bool ViewUnique = true;

    protected:
    private:
        ECS::ECS &m_ecs;
        GLFWwindow *m_window;
        std::unordered_map<int, FBOContext> m_view_fbos;

        // Move guizmo
        int m_drag_axis = -1; // -1 not dragging, 0/1/2 - X/Y/Z
        glm::vec3 m_drag_start_pos; // Entity's position when dragging started
        float m_drag_start_t; // Where on the axis the mouse was when the drag started
};

#endif /* !SCENEVIEWERPANEL_HPP_ */
