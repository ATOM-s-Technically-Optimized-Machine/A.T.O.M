#include <cfloat>
#include <renderer/glad/glad.h>
#include "core/MathUtils.hpp"
#include "tools/panels/SceneViewerPanel.hpp"
#include "imgui.h"
#include "inputs/ECSIntegration.hpp"
#include "renderer/RenderSystem.hpp"
#include "tools/Editor.hpp"
#include "tools/panels/APanel.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

static ImVec2 toScreen(
    const glm::vec4 &clip, 
    const Atom::Input::CameraComponent::ViewPort &vp,
    ImVec2 img_min,
    ImVec2 img_size
    )
{
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    float u = vp.x + (ndc.x * 0.5f + 0.5f) * vp.width;
    float v = vp.y + (ndc.y * 0.5f + 0.5f) * vp.height;
    return ImVec2(img_min.x + u * img_size.x, img_min.y + (1.0f - v) * img_size.y);
}

static float distToSegment(ImVec2 p, ImVec2 a, ImVec2 b)
{
    float abx = b.x - a.x, aby = b.y - a.y;
    float apx = p.x - a.x, apy = p.y - a.y;
    float len_sq = abx * abx + aby * aby;
    float t = len_sq > 0.0f ? std::clamp((apx * abx + apy * aby) / len_sq, 0.0f, 1.0f) : 0.0f;
    float dx = apx - abx * t, dy = apy - aby * t;
    return std::sqrt(dx * dx + dy * dy);
}

static ImU32 withAlpha(ImU32 col, int alpha)
{
    return (col & ~IM_COL32_A_MASK) | ((ImU32)alpha << IM_COL32_A_SHIFT);
}

static int hoveredAxis(const MoveGizmoScreen &g, ImVec2 mouse)
{
    if (!g.visible)
        return -1;
    const float threshold = 6.0f;
    int best = -1;
    float best_d = threshold;

    for (int i = 0; i < 3; i++) {
        if (!g.axis_visibles[i])
            continue;
        float d = distToSegment(mouse, g.origin, g.tips[i]);
        if (d < best_d) {
            best_d = d;
            best = i;
        }
    }
    return best;
}

static Ray mouseRay(Atom::Input::CameraComponent &cam, ImVec2 img_min, ImVec2 img_size)
{
    ImVec2 m = ImGui::GetMousePos();
    float u = (m.x - img_min.x) / img_size.x;
    float v = 1.0f - (m.y - img_min.y) / img_size.y;

    const auto &vp = cam.viewport;
    float ndc_x = ((u - vp.x) / vp.width) * 2.0f - 1.0f;
    float ndc_y = ((v - vp.y) / vp.height) * 2.0f - 1.0f;

    glm::mat4 inv_vp = glm::inverse(cam.camera.viewProjectionMatrix());

    glm::vec4 near_pt = inv_vp * glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);
    glm::vec4 far_pt = inv_vp * glm::vec4(ndc_x, ndc_y, 1.0f, 1.0f);
    near_pt /= near_pt.w;
    far_pt /= far_pt.w;

    glm::vec3 origin = glm::vec3(near_pt);
    glm::vec3 dir = glm::normalize(glm::vec3(far_pt - near_pt));

    return Ray(origin, dir);
}

static bool rayAABB(Ray ray, glm::vec3 bmin, glm::vec3 bmax, float &t_hit)
{
    float t_near = -FLT_MAX;
    float t_far = FLT_MAX;

    for (int a = 0; a < 3; a++) {
        if (std::abs(ray.direction[a]) < 1e-8f) {
            if (ray.origin[a] < bmin[a] || ray.origin[a] > bmax[a])
                return false;
            continue;
        }

        float t1 = (bmin[a] - ray.origin[a]) / ray.direction[a];
        float t2 = (bmax[a] - ray.origin[a]) / ray.direction[a];
        if (t1 > t2)
            std::swap(t1, t2);

        t_near = std::max(t_near, t1);
        t_far = std::min(t_far, t2);

        if (t_near > t_far)
            return false;
    }

    if (t_far < 0.0f) return false;
    t_hit = std::max(t_near, 0.0f);
    return true;
}

static bool rayHitsEntity(ECS::ECS &ecs, ECS::EntityID e, const Ray &ray, float &t_hit)
{
    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 bmin(-0.5f), bmax(0.5f);

    if (ecs.entityHasComponent<MeshHandleComponent>(e)) {
        auto &mesh = ecs.entityGetComponent<MeshHandleComponent>(e);
        bmin = mesh.boundsMin;
        bmax = mesh.boundsMax;
    }

    // Move the ray into the etity's local space (where its box is axis aligned)
    // The direction is NOT normalized so t stays a world-space distance
    glm::mat4 inv = TransformComponent::toLocal(transform);
    Ray local(transformPoint(inv, ray.origin), transformDirection(inv, ray.direction));

    return rayAABB(local, bmin, bmax, t_hit);
}

static ECS::EntityID pickEntity(ECS::ECS &ecs, const Ray &ray)
{
    std::vector<ECS::EntityID> candidates;
    ecs.getEntitiesByComponentsAllOf<TransformComponent, MeshHandleComponent>(candidates);

    ECS::EntityID best = ECS::NULL_ENTITY;
    float best_t = FLT_MAX;

    for (auto e : candidates) {
        float t;
        if (rayHitsEntity(ecs, e, ray, t) && t < best_t) {
            best_t = t;
            best = e;
        }
    }
    return best;
}

// Draws `e`'s local bounds as a wireframe box over the viewport image.
// Mesh entities use their mesh bounds, others a unit cube. Edges are clipped
// against the near plane in clip space, so a box partly behind the camera
// doesn't produce wrapped-around lines.
static void drawSelectionBox(
    ECS::ECS &ecs,
    ECS::EntityID e,
    Atom::Input::CameraComponent &cam,
    ImVec2 img_min,
    ImVec2 img_size,
    uint32_t color = IM_COL32(255, 160, 0, 255)
)
{
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return;

    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 bmin(-0.5f), bmax(0.5f);
    if (ecs.entityHasComponent<MeshHandleComponent>(e)) {
        auto &mesh = ecs.entityGetComponent<MeshHandleComponent>(e);
        bmin = mesh.boundsMin;
        bmax = mesh.boundsMax;
    }

    glm::mat4 mvp = cam.camera.viewProjectionMatrix() * TransformComponent::toWorld(transform);

    // Corner i takes bmax on axis k when bit k of i is set
    glm::vec4 corners[8];
    for (int i = 0; i < 8; i++) {
        glm::vec3 p(
            (i & 1) ? bmax.x : bmin.x,
            (i & 2) ? bmax.y : bmin.y,
            (i & 4) ? bmax.z : bmin.z
        );
        corners[i] = mvp * glm::vec4(p, 1.0f);
    }

    const auto &vp = cam.viewport;

    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(img_min, ImVec2(img_min.x + img_size.x, img_min.y + img_size.y), true);

    for (int i = 0; i < 8; i++) {
        for (int bit = 1; bit < 8; bit <<= 1) {
            if (i & bit)
                continue;
            glm::vec4 a = corners[i];
            glm::vec4 b = corners[i | bit];
            // Signed distance to the near plane (z = -w); negative is behind
            float da = a.z + a.w;
            float db = b.z + b.w;
            if (da < 0.0f && db < 0.0f)
                continue;
            if (da < 0.0f)
                a += (b - a) * (da / (da - db));
            else if (db < 0.0f)
                b += (a - b) * (db / (db - da));
            draw->AddLine(toScreen(a, vp, img_min, img_size), toScreen(b, vp, img_min, img_size), color, 1.5f);
        }
    }

    draw->PopClipRect();
}

static void drawMoveGizmo(
    MoveGizmoScreen g,
    int hovered_axis,
    ImVec2 img_min,
    ImVec2 img_size
)
{
    const ImU32 colors[3] = { IM_COL32(230,60,60,255), IM_COL32(60,200,60,255), IM_COL32(60,110,230,255) };

    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(img_min, ImVec2(img_min.x + img_size.x, img_min.y + img_size.y), true);

    for (int i = 0; i < 3; i++) {
        float dx = g.tips[i].x - g.origin.x;
        float dy = g.tips[i].y - g.origin.y;
        float length = std::sqrt(dx * dx + dy * dy);

        if (!g.axis_visibles[i])
            continue;

        ImVec2 dir(dx / length, dy / length);
        ImVec2 perp(-dir.y, dir.x);

        const float head_length = 12.0f;
        const float head_width = 5.0f;
        
        ImVec2 tip = g.tips[i];
        ImVec2 base(tip.x - dir.x * head_length, tip.y - dir.y * head_length);
        ImVec2 right(base.x + perp.x * head_width, base.y + perp.y * head_width);
        ImVec2 left(base.x - perp.x * head_width, base.y - perp.y * head_width);

        bool hot = (i == hovered_axis);
        ImColor col = hot ? IM_COL32(255, 220, 80, 255) : colors[i];

        if (hot) {
            const float widths[] = {12.0f, 8.0f};
            const int alphas[] = {40, 90};
            for (int k = 0; k < 2; k++) {
                ImVec2 circle_center(g.tips[i].x - (head_length * dir.x * 0.7f), g.tips[i].y - (head_length * dir.y * 0.7f));
                draw->AddLine(g.origin, g.tips[i], withAlpha(col, alphas[k]), widths[k]);
                draw->AddCircleFilled(circle_center, widths[k] * 0.9f, withAlpha(col, alphas[k]));
            }
        }

        draw->AddLine(g.origin, base, col, hot ? 4.0f : 3.0f);
        draw->AddTriangleFilled(tip, left, right, col);
    }

    draw->PopClipRect();
}

static MoveGizmoScreen computeMoveGizmo(ECS::ECS &ecs, ECS::EntityID e, Atom::Input::CameraComponent &cam, ImVec2 img_min, ImVec2 img_size)
{
    MoveGizmoScreen g;
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return g;

    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 origin = transform.position;
    float len = glm::distance(cam.camera.position(), origin) * 0.15f;

    glm::mat4 vp_mat = cam.camera.viewProjectionMatrix();
    glm::vec4 o_clip = vp_mat * glm::vec4(origin, 1.0f);

    if (o_clip.w <= 0.0f)
        return g;

    g.origin = toScreen(o_clip, cam.viewport, img_min, img_size);
    g.visible = true;

    const glm::vec3 axes[3] = { {1,0,0}, {0,1,0}, {0,0,1} };

    for (int i = 0; i < 3; i++) {
        glm::vec4 end_clip = vp_mat * glm::vec4(origin + axes[i] * len, 1.0f);
        if (end_clip.w <= 0)
            continue;
        ImVec2 end_px = toScreen(end_clip, cam.viewport, img_min, img_size);

        float dx = end_px.x - g.origin.x;
        float dy = end_px.y - g.origin.y;
        float length = std::sqrt(dx * dx + dy * dy);

        if (length < 1.0f)
            continue;
        g.tips[i] = end_px;
        g.axis_visibles[i] = true;
    }
    return g;
}

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
    
    ECS::EntityID sel = m_editor.getSelectedEntity();
    
    if (m_drag_axis != -1 && sel != ECS::NULL_ENTITY) {
        Ray ray = mouseRay(cam, img_min, size);
        glm::vec3 w0 = m_drag_start_pos - ray.origin;
        float b = glm::dot(Axes[m_drag_axis], ray.direction), d = glm::dot(Axes[m_drag_axis], w0), e = glm::dot(ray.direction, w0);
        float denom = 1.0f - b * b;
        float t = (b * e - d) / denom;
        auto &transform = m_ecs.entityGetComponent<TransformComponent>(sel);
        if (std::abs(denom) >= 1e-4f)
            transform.position = m_drag_start_pos + Axes[m_drag_axis] * (t - m_drag_start_t);
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_drag_axis = -1;
    }

    MoveGizmoScreen gizmo = computeMoveGizmo(m_ecs, sel, cam, img_min, size);

    int hovered_axis = -1;
    if (hovered && !m_captured && m_drag_axis == -1)
        hovered_axis = hoveredAxis(gizmo, ImGui::GetMousePos());
    
    if (hovered_axis != -1)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ECS::EntityID hovered_entity = ECS::NULL_ENTITY;
    if (hovered && !m_captured && hovered_axis == -1 && m_drag_axis == -1)
        hovered_entity = pickEntity(m_ecs, mouseRay(cam, img_min, size));
    
    if (hovered && !m_captured && hovered_axis == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        m_editor.setSelectedEntity(hovered_entity);

    if (hovered && !m_captured && hovered_axis != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && sel != ECS::NULL_ENTITY) {
        m_drag_start_pos = m_ecs.entityGetComponent<TransformComponent>(sel).position;
        Ray ray = mouseRay(cam, img_min, size);
        glm::vec3 w0 = m_drag_start_pos - ray.origin;
        float b = glm::dot(Axes[hovered_axis], ray.direction), d = glm::dot(Axes[hovered_axis], w0), e = glm::dot(ray.direction, w0);
        float denom = 1.0f - b * b;
        if (denom >= 1e-4f) {
            float t = (b * e - d) / denom;
            m_drag_start_t = t;
            m_drag_axis = hovered_axis;
        }
    }

    if (hovered_entity != sel)
        drawSelectionBox(m_ecs, hovered_entity, cam, img_min, size, IM_COL32(255, 255, 255, 120));
    drawSelectionBox(m_ecs, sel, cam, img_min, size);
    drawMoveGizmo(gizmo, m_drag_axis == -1 ? hovered_axis : m_drag_axis, img_min, size);
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
