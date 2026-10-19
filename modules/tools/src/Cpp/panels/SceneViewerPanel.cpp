#include <cfloat>
#include <renderer/glad/glad.h>
#include "tools/panels/SceneViewerPanel.hpp"
#include "imgui.h"
#include "inputs/ECSIntegration.hpp"
#include "renderer/RenderSystem.hpp"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"

SceneViewerPanel::SceneViewerPanel(editor::Editor &editor, GLFWwindow *window, ECS::ECS &ecs)
: APanel(editor), m_ecs(ecs)
{
    m_name = "Scene preview";
    m_window = window;
}

void SceneViewerPanel::init()
{
}

SceneViewerPanel::~SceneViewerPanel()
{
}

void SceneViewerPanel::render(int view_id)
{
    if (m_captured)
        ImGui::GetIO().AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = (int)size.x, h = (int)size.y;

    if (w <= 0 || h <= 0)   // Nothing to display
        return;

    RenderSystem &render_sys = m_ecs.getSystem<RenderSystem>();
    FBOContext &fbo = m_view_fbos[view_id];

    if (w != fbo.w || h != fbo.h) {
        if (fbo.fbo) {
            glDeleteFramebuffers(1, &fbo.fbo);
            glDeleteTextures(1, &fbo.tex);
            glDeleteRenderbuffers(1, &fbo.depth);
        }

        fbo.w = w;
        fbo.h = h;
        editor::genFBO(fbo.w, fbo.h, fbo.fbo, fbo.tex, fbo.depth);
        render_sys.setTargetFBO(fbo.fbo, fbo.w, fbo.h);
    }

    ImGui::Image((ImTextureID)(intptr_t)fbo.tex, size, ImVec2(0, 1), ImVec2(1, 0));

    bool focused = ImGui::IsWindowFocused();
    bool hovered = ImGui::IsWindowHovered();
    bool should_capture = focused && hovered;

    if (should_capture && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        m_captured = true;
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (m_captured) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        m_captured = false;
    }

    std::vector<ECS::EntityID> cameras;
    m_ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
    ECS::EntityID cam_id = cameras[0];
    auto &cam = m_ecs.entityGetComponent<Atom::Input::CameraComponent>(cam_id);
    cam.enabled = m_captured ? true : should_capture;

}

void SceneViewerPanel::contextMenu(int view_id)
{
}

void SceneViewerPanel::onViewClose(int view_id)
{
    auto it = m_view_fbos.find(view_id);
    if (it == m_view_fbos.end()) return;
    auto &f = it->second;
    glDeleteFramebuffers(1, &f.fbo);
    glDeleteTextures(1, &f.tex);
    glDeleteRenderbuffers(1, &f.depth);
    m_view_fbos.erase(it);
}
