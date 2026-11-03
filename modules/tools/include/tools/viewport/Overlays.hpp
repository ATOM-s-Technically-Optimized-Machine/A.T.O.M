#ifndef OVERLAYS_HPP_
#define OVERLAYS_HPP_

#include <imgui.h>
#include <glm/glm.hpp>
#include "tools/viewport/Picking.hpp"
#include "tools/viewport/ViewportContext.hpp"

// Draws `e`'s local bounds as a wireframe box over the viewport image.
// Mesh entities use their mesh bounds, others a unit cube. Edges are clipped
// against the near plane in clip space, so a box partly behind the camera
// doesn't produce wrapped-around lines
inline void drawSelectionBox(ECS::ECS &ecs, ECS::EntityID e, const ViewportContext &view, uint32_t color = IM_COL32(255, 160, 0, 255))
{
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return;

    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 bmin(-0.5f), bmax(0.5f);
    getEntityBounds(ecs, e, bmin, bmax);

    glm::mat4 mvp = view.view_proj * TransformComponent::toWorld(transform);

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

    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(view.min, ImVec2(view.min.x + view.size.x, view.min.y + view.size.y), true);

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
            draw->AddLine(toScreen(a, view), toScreen(b, view), color, 1.5f);
        }
    }

    draw->PopClipRect();
}

#endif /* OVERLAYS_HPP_ */