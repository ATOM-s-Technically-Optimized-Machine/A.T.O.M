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
    bool hovered = ImGui::IsWindowHovered();
    bool should_capture = focused && hovered;

    if (should_capture && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        m_captured = true;
        m_editor.setCursorCaptured(true);
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (m_captured)
            m_editor.setCursorCaptured(false);
        m_captured = false;
    }

    std::vector<ECS::EntityID> cameras;
    m_ecs.getEntitiesByComponentsAllOf<Atom::Input::CameraComponent>(cameras);
    ECS::EntityID cam_id = cameras[0];
    auto &cam = m_ecs.entityGetComponent<Atom::Input::CameraComponent>(cam_id);
    cam.enabled = m_captured ? true : should_capture;

    ViewportContext view(cam, img_min, size);
    
    ECS::EntityID sel = m_editor.getSelectedEntity();
    bool dragging = m_drag_axis != -1 || m_drag_ring != -1;

    if (focused && !m_captured && !dragging && sel != ECS::NULL_ENTITY) {
        if (ImGui::IsKeyPressed(ImGuiKey_M, false))
           m_tool = m_tool == EditTool::Move ? EditTool::Rotate : EditTool::Move;
        if (ImGui::IsKeyPressed(ImGuiKey_L, false))
            m_move_local = !m_move_local;
    }
    
    if (m_drag_axis != -1 && sel != ECS::NULL_ENTITY) {
        Ray ray = mouseRay(view);
        glm::vec3 w0 = m_drag_start_pos - ray.origin;
        float b = glm::dot(m_drag_dir, ray.direction), d = glm::dot(m_drag_dir, w0), e = glm::dot(ray.direction, w0);
        float denom = 1.0f - b * b;
        float t = (b * e - d) / denom;
        auto &transform = m_ecs.entityGetComponent<TransformComponent>(sel);
        if (std::abs(denom) >= 1e-4f)
            transform.position = m_drag_start_pos + m_drag_dir * (t - m_drag_start_t);
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_drag_axis = -1;
    }

    if (m_drag_ring != -1 && sel != ECS::NULL_ENTITY) {
        auto &transform = m_ecs.entityGetComponent<TransformComponent>(sel);
        glm::vec3 axis = m_drag_start_rot * Axes[m_drag_ring];
        glm::vec3 hit;
        if (rayPlane(mouseRay(view), transform.position, axis, hit)) {
            glm::vec3 now = hit - transform.position;
            float angle = std::atan2(glm::dot(glm::cross(m_drag_start_vec, now), axis), glm::dot(m_drag_start_vec, now));
            transform.rotation = glm::angleAxis(angle, axis) * m_drag_start_rot;
        }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_drag_ring = -1;
    }

    MoveGizmoScreen gizmo = computeMoveGizmo(m_ecs, sel, view, m_tool == EditTool::Rotate || m_move_local);
    RotateGizmoScreen rings;

    if (m_tool == EditTool::Rotate)
        rings = computeRotateGizmo(m_ecs, sel, view);

    int hovered_axis = -1;
    if (m_tool == EditTool::Move && hovered && !m_captured && !dragging)
        hovered_axis = hoveredAxis(gizmo, ImGui::GetMousePos());
    
    int hovered_ring = -1;
    if (m_tool == EditTool::Rotate && hovered && !m_captured && !dragging)
        hovered_ring = hoveredRing(rings, ImGui::GetMousePos());

    bool over_gizmo = hovered_axis != -1 || hovered_ring != -1;
    if (over_gizmo)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ECS::EntityID hovered_entity = ECS::NULL_ENTITY;
    if (hovered && !m_captured && !over_gizmo && !dragging)
        hovered_entity = pickEntity(m_ecs, mouseRay(view));
    
    if (hovered && !m_captured && !over_gizmo && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        m_editor.setSelectedEntity(hovered_entity);

    if (hovered && !m_captured && hovered_axis != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && sel != ECS::NULL_ENTITY) {
        auto &transform = m_ecs.entityGetComponent<TransformComponent>(sel);
        glm::vec3 dir = m_move_local ? glm::normalize(transform.rotation) * Axes[hovered_axis] : Axes[hovered_axis];
        m_drag_start_pos = transform.position;
        Ray ray = mouseRay(view);
        glm::vec3 w0 = m_drag_start_pos - ray.origin;
        float b = glm::dot(dir, ray.direction), d = glm::dot(dir, w0), e = glm::dot(ray.direction, w0);
        float denom = 1.0f - b * b;
        if (denom >= 1e-4f) {
            float t = (b * e - d) / denom;
            m_drag_start_t = t;
            m_drag_dir = dir;
            m_drag_axis = hovered_axis;
        }
    }

    if (hovered && !m_captured && hovered_ring != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && sel != ECS::NULL_ENTITY) {
        auto &transform = m_ecs.entityGetComponent<TransformComponent>(sel);
        glm::quat rot = glm::normalize(transform.rotation);
        glm::vec3 axis = rot * Axes[hovered_ring];
        glm::vec3 hit;
        if (rayPlane(mouseRay(view), transform.position, axis, hit)) {
            m_drag_start_rot = rot;
            m_drag_start_vec = hit - transform.position;
            m_drag_ring = hovered_ring;
        }
    }

    if (hovered_entity != sel)
        drawSelectionBox(m_ecs, hovered_entity, view, IM_COL32(255, 255, 255, 120));
    drawSelectionBox(m_ecs, sel, view);

    if (m_tool == EditTool::Rotate)
        drawRotateGizmo(rings, m_drag_ring == -1 ? hovered_ring : m_drag_ring, view);
    drawMoveGizmo(gizmo, m_drag_axis == -1 ? hovered_axis : m_drag_axis, view);
    
    const char *label = m_tool == EditTool::Rotate
        ? "Rotate (M)"
        : m_move_local
            ? "Move (M) Local (L)"
            : "Move (M) Global (L)";
    ImGui::GetWindowDrawList()->AddText(ImVec2(img_min.x + 8, img_min.y + 8), IM_COL32_WHITE, label);
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
