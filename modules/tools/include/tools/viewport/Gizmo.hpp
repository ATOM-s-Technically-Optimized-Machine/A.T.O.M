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

struct Segment {
    ImVec2 a;
    ImVec2 b;
};

enum class HandleStyle { Arrow, Ring };

struct Handle {
    int axis;
    ImU32 color;
    HandleStyle style;
    std::vector<Segment> segments;
};

inline constexpr int RingSegments = 64;

inline float gizmoLength(Atom::Input::CameraComponent &cam, glm::vec3 origin)
{
    return glm::distance(cam.camera.position(), origin) * 0.15;
}

const Handle *hoveredHandle(const std::vector<Handle> &handles, ImVec2 mouse);
void drawHandles(const std::vector<Handle> &handles, int hot_axis, const ViewportContext &view);

inline float distToSegment(ImVec2 p, const Segment &s)
{
    float abx = s.b.x - s.a.x, aby = s.b.y - s.a.y;
    float apx = p.x - s.a.x, apy = p.y - s.a.y;
    float len_sq = abx * abx + aby * aby;
    float t = len_sq > 0.0f ? std::clamp((apx * abx + apy * aby) / len_sq, 0.0f, 1.0f) : 0.0f;
    float dx = apx - abx * t, dy = apy - aby * t;
    return std::sqrt(dx * dx + dy * dy);
}

inline ImU32 withAlpha(ImU32 col, int alpha)
{
    return (col & ~IM_COL32_A_MASK) | ((ImU32)alpha << IM_COL32_A_SHIFT);
}

#endif /* GIZMO_HPP_ */