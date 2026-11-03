#ifndef GIZMO_HPP_
#define GIZMO_HPP_

#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "core/DefaultComponents.hpp"
#include "core/ECS/ECS.hpp"
#include "tools/viewport/ViewportContext.hpp"

inline constexpr glm::vec3 Axes[3] = {{1,0,0}, {0,1,0}, {0,0,1}};
inline constexpr ImU32 AxisColors[3] = { IM_COL32(230,60,60,255), IM_COL32(60,200,60,255), IM_COL32(60,110,230,255) };

struct MoveGizmoScreen {
    bool visible = false;
    ImVec2 origin;
    ImVec2 tips[3];
    bool axis_visibles[3] = {};
};

inline constexpr int RingSegments = 64;

struct RotateGizmoScreen {
    ImVec2 points[3][RingSegments];
    bool points_visibles[3][RingSegments] = {};
};

inline float gizmoLength(Atom::Input::CameraComponent &cam, glm::vec3 origin)
{
    return glm::distance(cam.camera.position(), origin) * 0.15;
}

inline float distToSegment(ImVec2 p, ImVec2 a, ImVec2 b)
{
    float abx = b.x - a.x, aby = b.y - a.y;
    float apx = p.x - a.x, apy = p.y - a.y;
    float len_sq = abx * abx + aby * aby;
    float t = len_sq > 0.0f ? std::clamp((apx * abx + apy * aby) / len_sq, 0.0f, 1.0f) : 0.0f;
    float dx = apx - abx * t, dy = apy - aby * t;
    return std::sqrt(dx * dx + dy * dy);
}

inline ImU32 withAlpha(ImU32 col, int alpha)
{
    return (col & ~IM_COL32_A_MASK) | ((ImU32)alpha << IM_COL32_A_SHIFT);
}

inline int hoveredAxis(const MoveGizmoScreen &g, ImVec2 mouse)
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

inline int hoveredRing(const RotateGizmoScreen &g, ImVec2 mouse)
{
    const float threshold = 6.0f;
    int best = -1;
    float best_d = threshold;

    for (int i = 0; i < 3; i++) {
        for (int s = 0; s < RingSegments; s++) {
            int next = (s + 1) % RingSegments;
            if (!g.points_visibles[i][s] || !g.points_visibles[i][next])
                continue;
            float d = distToSegment(mouse, g.points[i][s], g.points[i][next]);
            if (d < best_d) {
                best_d = d;
                best = i;
            }
        }
    }
    return best;
}

inline MoveGizmoScreen computeMoveGizmo(ECS::ECS &ecs, ECS::EntityID e, const ViewportContext &view, bool local)
{
    MoveGizmoScreen g;
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return g;

    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 origin = transform.position;
    float len = gizmoLength(view.cam, origin);

    glm::mat4 vp_mat = view.view_proj;
    glm::vec4 o_clip = vp_mat * glm::vec4(origin, 1.0f);

    if (o_clip.w <= 0.0f)
        return g;
    
    g.origin = toScreen(o_clip, view);
    g.visible = true;

    for (int i = 0; i < 3; i++) {
        glm::vec3 axis = local ? transform.rotation * Axes[i] : Axes[i];
        glm::vec4 end_clip = vp_mat * glm::vec4(origin + axis * len, 1.0f);
        if (end_clip.w <= 0)
            continue;
        ImVec2 end_px = toScreen(end_clip, view);

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

inline RotateGizmoScreen computeRotateGizmo(ECS::ECS &ecs, ECS::EntityID e, const ViewportContext &view)
{
    RotateGizmoScreen g;
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return g;
    
    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 origin = transform.position;
    float radius = gizmoLength(view.cam, origin);
    glm::mat4 vp_mat = view.view_proj;

    for (int i = 0; i < 3; i++) {
        glm::vec3 u = transform.rotation * Axes[(i + 1) % 3];
        glm::vec3 v = transform.rotation * Axes[(i + 2) % 3];

        for (int s = 0; s < RingSegments; s++) {
            float a = glm::two_pi<float>() * s / RingSegments;
            glm::vec3 p = origin + (std::cos(a) * u + std::sin(a) * v) * radius;
            if (project(view, p, g.points[i][s]))
                g.points_visibles[i][s] = true;
        }
    }
    return g;
}

inline void drawMoveGizmo(MoveGizmoScreen g, int hovered_axis, const ViewportContext &view)
{
    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(view.min, ImVec2(view.min.x + view.size.x, view.min.y + view.size.y), true);

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
        ImColor col = hot ? IM_COL32(255, 220, 80, 255) : AxisColors[i];

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

inline void drawRotateGizmo(const RotateGizmoScreen &g, int hot_ring, const ViewportContext &view)
{
    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(view.min, ImVec2(view.min.x + view.size.x, view.min.y + view.size.y), true);

    for (int i = 0; i < 3; i++) {
        bool hot = (i == hot_ring);
        ImU32 col = hot ? IM_COL32(255, 220, 80, 255) : AxisColors[i];

        for (int s = 0; s < RingSegments; s++) {
            int next = (s + 1) % RingSegments;
            if (g.points_visibles[i][s] && g.points_visibles[i][next])
                draw->AddLine(g.points[i][s], g.points[i][next], col, hot ? 4.0f : 2.5f);
        }
    }
    draw->PopClipRect();
}

#endif /* GIZMO_HPP_ */