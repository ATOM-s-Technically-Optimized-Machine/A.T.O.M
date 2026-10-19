#ifndef SCENEVIEWERPANEL_HPP_
#define SCENEVIEWERPANEL_HPP_

#include "core/ECS/ECS.hpp"
#include "tools/panels/APanel.hpp"
#include "tools/panels/IPanel.hpp"
#include <GLFW/glfw3.h>

struct FBOContext {
    unsigned int fbo = 0;
    unsigned int tex = 0;
    unsigned int depth = 0;
    int w = 0;
    int h = 0;
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
};

#endif /* !SCENEVIEWERPANEL_HPP_ */
