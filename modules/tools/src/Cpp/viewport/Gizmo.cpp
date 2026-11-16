#include "tools/viewport/Gizmo.hpp"

const Handle *hoveredHandle(const std::vector<Handle> &handles, ImVec2 mouse)
{
    const float threshold = 6.0f;
    const Handle *best = nullptr;
    float best_d = threshold;

    for (int i = 0; i < handles.size(); i++) {
        const auto &handle = handles[i];

        for (const auto &segment : handle.segments) {
            float d = distToSegment(mouse, segment);
            if (d < best_d) {
                best_d = d;
                best = &handles[i];
            }
        }

        // Treat a box as a disc: its screen centre, with a radius equal to the
        // average centre-to-corner distance. The score is the distance to the
        // disc's edge (negative inside), so the whole box is grabbable and a
        // box under the cursor beats a nearby arrow.
        if (handle.style == HandleStyle::Box && !handle.points.empty()) {
            ImVec2 center(0.0f, 0.0f);
            for (const auto &p : handle.points) {
                center.x += p.x;
                center.y += p.y;
            }
            center.x /= handle.points.size();
            center.y /= handle.points.size();

            float radius = 0.0f;
            for (const auto &p : handle.points)
                radius += std::hypot(p.x - center.x, p.y - center.y);
            radius /= handle.points.size();

            float d = std::hypot(mouse.x - center.x, mouse.y - center.y) - radius;
            if (d < best_d) {
                best_d = d;
                best = &handles[i];
            }
        }
    }
    return best;
}

static void drawArrow(const Segment &segment, ImU32 color, ImDrawList *draw, bool hot = false)
{
    const float head_length = 12.0f;
    const float head_width = 5.0f;

    float dx = segment.b.x - segment.a.x;
    float dy = segment.b.y - segment.a.y;
    float length = std::sqrt(dx * dx + dy * dy);

    ImVec2 dir(dx / length, dy / length);
    ImVec2 perp(-dir.y, dir.x);

    ImVec2 base(segment.b.x - dir.x * head_length, segment.b.y - dir.y * head_length);

    ImVec2 left(base.x + perp.x * head_width, base.y + perp.y * head_width);
    ImVec2 right(base.x - perp.x * head_width, base.y - perp.y * head_width);

    draw->AddLine(segment.a, ImVec2(segment.b.x - dir.x * head_length, segment.b.y - dir.y * head_length), color, hot ? 4.5 : 2.5);
    draw->AddTriangleFilled(segment.b, left, right, color);
}

static void drawBox(const Handle &handle, ImDrawList *draw, ImU32 color)
{
    const std::vector<ImVec2> &p = handle.points;
    for (int k = 0; k < 3; k++) {
        int a = 1 << ((k + 1) % 3);
        int b = 1 << ((k + 2) % 3);
        for (int side = 0; side < 2; side++) {
            int base = side ? (1 << k) : 0;
            draw->AddQuadFilled(p[base], p[base | a], p[base | a | b], p[base | b], color);
        }
    }
}

void drawHandles(const std::vector<Handle> &handles, const ViewportContext &view)
{
    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(view.min, ImVec2(view.min.x + view.size.x, view.min.y + view.size.y), true);

    for (const auto &handle : handles) {
        ImU32 color = handle.hot ? IM_COL32(255, 220, 80, 255) : handle.color;
        switch (handle.style) {
            case HandleStyle::Arrow:
                for (const auto &segment : handle.segments)
                    drawArrow(segment, color, draw, handle.hot);
                break;
            case HandleStyle::Ring:
                for (const auto &segment : handle.segments)
                    draw->AddLine(segment.a, segment.b, color, handle.hot ? 4.5 : 2.5);
                break;
            case HandleStyle::Box:
                drawBox(handle, draw, color);
                break;
            default:
                break;
        }

    }
    draw->PopClipRect();
}
