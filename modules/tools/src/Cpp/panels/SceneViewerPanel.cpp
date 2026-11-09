#include <cfloat>
#include <renderer/glad/glad.h>
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "core/MathUtils.hpp"
#include "tools/panels/SceneViewerPanel.hpp"
#include "imgui.h"
#include "inputs/ECSIntegration.hpp"
#include "renderer/RenderSystem.hpp"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
#include "tools/viewport/ViewportContext.hpp"
#include "tools/viewport/Picking.hpp"
#include "tools/viewport/Gizmo.hpp"
#include "tools/viewport/Overlays.hpp"

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
    ImVec2 img_min = ImGui::GetItemRectMin();

    bool focused = ImGui::IsWindowFocused();
    bool hovered_window = ImGui::IsWindowHovered();
    bool should_capture = focused && hovered_window;

    if (should_capture && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        m_captured = true;
        m_editor.setCursorCaptured(true);
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (m_captured)
            m_editor.setCursorCaptured(false);
        m_captured = false;
    }

    // Get camera
    std::vector<ECS::EntityID> cameras;
    m_ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
    ECS::EntityID cam_id = cameras[0];
    auto &cam = m_ecs.entityGetComponent<Atom::Input::CameraComponent>(cam_id);
    cam.enabled = m_captured ? true : should_capture;

    // Get view and active tool
    ViewportContext view(cam, img_min, size);
    ITool *tool = m_editor.getActiveTool();

    // Get selected entity and drag state
    ECS::EntityID sel = m_editor.getSelectedEntity();
    bool dragging = m_drag_entity != ECS::NULL_ENTITY;

    // Update drag state
    if (dragging && (!tool || !tool->updateDrag(mouseRay(view), m_ecs, m_drag_entity) || !ImGui::IsMouseDown(ImGuiMouseButton_Left)))
        m_drag_entity = ECS::NULL_ENTITY;

    // Building gizmo handles
    std::vector<Handle> handles;
    if (tool && m_ecs.entityIsActive(sel))
        tool->buildHandles(view, m_ecs, sel, handles);
    
    // Checking hovered handle and storing which axis it is
    const Handle *hovered = (hovered_window && !m_captured && !dragging) ? hoveredHandle(handles, ImGui::GetMousePos()) : nullptr;
    for (auto &handle : handles)
        if (&handle == hovered || (dragging && handle.axis == m_drag_axis))
            handle.hot = true;

    // Change mouse cursor if hovering
    if (hovered)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    // Get the hovered entity if gizmo isn't hovered
    ECS::EntityID hovered_entity = ECS::NULL_ENTITY;
    if (hovered_window && !m_captured && !hovered && !dragging)
        hovered_entity = pickEntity(m_ecs, mouseRay(view));
    
    // If clicked while hovering entity, select entity
    if (hovered_window && !m_captured && !hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        m_editor.setSelectedEntity(hovered_entity);

    // Start dragging if gizmo hovered and mouse clicked
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (tool->beginDrag(*hovered, mouseRay(view), m_ecs, sel)) {
            m_drag_entity = sel;
            m_drag_axis = hovered->axis;
        }
    }

    // Draw boxes around selected/hovered entities
    if (hovered_entity != sel)
        drawSelectionBox(m_ecs, hovered_entity, view, IM_COL32(255, 255, 255, 120));
    drawSelectionBox(m_ecs, sel, view);

    drawHandles(handles, view);
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
