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

void drawHandles(const std::vector<Handle> &handles, int hot_axis, const ViewportContext &view)
{
    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->PushClipRect(view.min, ImVec2(view.min.x + view.size.x, view.min.y + view.size.y), true);

    for (const auto &handle : handles) {
        bool hot = handle.axis == hot_axis;
        ImU32 color = hot ? IM_COL32(255, 220, 80, 255) : handle.color;
        for (const auto &segment : handle.segments) {
            if (handle.style == HandleStyle::Arrow)
                drawArrow(segment, color, draw, hot);
            else
                draw->AddLine(segment.a, segment.b, color, hot ? 4.5 : 2.5);
        }
    }

    draw->PopClipRect();
}
