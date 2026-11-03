#include "tools/edit/TransformTools.hpp"
#include "tools/viewport/ViewportContext.hpp"
#include "tools/viewport/Gizmo.hpp"
#include "core/DefaultComponents.hpp"

void MoveTool::buildHandles(const ViewportContext &view, const TransformComponent &transform, std::vector<Handle> &out) const
{
    ImVec2 origin;
    if (!project(view, transform.position, origin))
        return;
    
    float len = gizmoLength(view, transform.position);
    for (int i = 0; i < 3; i++) {
        glm::vec3 axis = m_local ? transform.rotation * Axes[i] : Axes[i];
        ImVec2 tip;
        if (!project(view, transform.position + axis * len, tip))
            continue;

        float dx = tip.x - origin.x, dy = tip.y - origin.y;
        if (std::sqrt(dx * dx + dy * dy) < 1.0f)
            continue;
        out.push_back({i, AxisColors[i], HandleStyle::Arrow, {{origin, tip}}});
    }
}

void RotateTool::buildHandles(const ViewportContext &view, const TransformComponent &transform, std::vector<Handle> &out) const
{
    float radius = gizmoLength(view, transform.position);

    for (int i = 0; i < 3; i++) {
        glm::vec3 u = transform.rotation * Axes[(i + 1) % 3];
        glm::vec3 v = transform.rotation * Axes[(i + 2) % 3];
        ImVec2 points[RingSegments];
        bool visible[RingSegments];

        for (int s = 0; s < RingSegments; s++) {
            float a = glm::two_pi<float>() * s / RingSegments;
            visible[s] = project(view, transform.position + (std::cos(a) * u + std::sin(a) * v) * radius, points[s]);
        }

        Handle ring(i, AxisColors[i], HandleStyle::Ring, {});
        for (int s = 0; s < RingSegments; s++) {
            int next = (s + 1) % RingSegments;
            if (visible[s] && visible[next])
                ring.segments.push_back({points[s], points[next]});
        }
        out.push_back(std::move(ring));
    }
}
