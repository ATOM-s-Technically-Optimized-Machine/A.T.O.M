#include "tools/edit/TransformTools.hpp"
#include "tools/viewport/ViewportContext.hpp"
#include "tools/viewport/Gizmo.hpp"
#include "tools/viewport/Picking.hpp"
#include "core/DefaultComponents.hpp"

static TransformComponent *getTransform(ECS::ECS &ecs, ECS::EntityID e)
{
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return nullptr;
    return &ecs.entityGetComponent<TransformComponent>(e);
}

// A small cube around `c`, aligned with the object's axes, sized to stay constant on screen
static void pushBox(const ViewportContext &view, glm::vec3 c, const glm::vec3 axes[3], int id, ImU32 color, std::vector<Handle> &out)
{
    float h = gizmoLength(view, c) * 0.08f;
    Handle box{id, color, HandleStyle::Box, {}, {}};
    for (int i = 0; i < 8; i++) {
        glm::vec3 s((i & 1) ? 1 : -1, (i & 2) ? 1 : -1, (i & 4) ? 1 : -1);
        ImVec2 p;
        if (!project(view, c + (axes[0] * s.x + axes[1] * s.y + axes[2] * s.z) * h, p))
            return;
        box.points.push_back(p);
    }
    out.push_back(box);
}

void MoveTool::buildHandles(const ViewportContext &view, ECS::ECS &ecs, ECS::EntityID e, std::vector<Handle> &out) const
{
    auto *t = getTransform(ecs, e);
    if (!t)
        return;
    
    auto &transform = *t;
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

bool MoveTool::beginDrag(const Handle &handle, const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    auto &transform = *raw_transform;
    glm::vec3 dir = m_local ? glm::normalize(transform.rotation) * Axes[handle.axis] : Axes[handle.axis];
    float t;
    if (!rayLineParam(ray, transform.position, dir, t))
        return false;

    m_dir = dir;
    m_start_pos = transform.position;
    m_start_t = t;
    return true;
}

bool MoveTool::updateDrag(const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    auto &transform = *raw_transform;
    float t;
    if (rayLineParam(ray, m_start_pos, m_dir, t)) {
        transform.position = m_start_pos + m_dir * (t - m_start_t);
    }
    return true;
}

void RotateTool::buildHandles(const ViewportContext &view, ECS::ECS &ecs, ECS::EntityID e, std::vector<Handle> &out) const
{
    auto *t = getTransform(ecs, e);
    if (!t)
        return;
    
    auto &transform = *t;
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

bool RotateTool::beginDrag(const Handle &handle, const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    auto &transform = *raw_transform;
    glm::quat rot = glm::normalize(transform.rotation);
    glm::vec3 axis = rot * Axes[handle.axis];
    glm::vec3 hit;
    if (!rayPlane(ray, transform.position, axis, hit))
        return false;
    
    m_axis = axis;
    m_start_rot = rot;
    m_start_vec = hit - transform.position;
    return true;
}

bool RotateTool::updateDrag(const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    auto &transform = *raw_transform;
    glm::vec3 hit;
    if (!rayPlane(ray, transform.position, m_axis, hit))
        return true;
    
    glm::vec3 now = hit - transform.position;
    float angle = std::atan2(glm::dot(glm::cross(m_start_vec, now), m_axis), glm::dot(m_start_vec, now));
    transform.rotation = glm::angleAxis(angle, m_axis) * m_start_rot;
    return true;
}

void ScaleTool::buildHandles(const ViewportContext &view, ECS::ECS &ecs, ECS::EntityID e, std::vector<Handle> &out) const
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return;
    
    auto &transform = *raw_transform;
    ImVec2 origin;
    if (!project(view, transform.position, origin))
        return;
    
    float len = gizmoLength(view, transform.position);
    
    for (int i = 0; i < 3; i++) {
        ImVec2 tip;
        if (!project(view, transform.position + transform.rotation * Axes[i] * len, tip))
            continue;

        float dx = tip.x - origin.x, dy = tip.y - origin.y;
        if (std::sqrt(dx * dx + dy * dy) < 1.0f)
            continue;
        out.push_back({i, AxisColors[i], HandleStyle::Arrow, {{origin, tip}}});
    }

    glm::vec3 bmin(-0.5f), bmax(0.5f);
    getEntityBounds(ecs, e, bmin, bmax);
    glm::mat4 model = TransformComponent::toWorld(transform);
    glm::vec3 axes[3] = {
        transform.rotation * Axes[0],
        transform.rotation * Axes[1],
        transform.rotation * Axes[2]
    };

    for (int i = 0; i < 8; i++) {
        glm::vec3 local((i & 1) ? bmax.x : bmin.x, (i & 2) ? bmax.y : bmin.y, (i & 4) ? bmax.z : bmin.z);
        glm::vec3 world = glm::vec3(model * glm::vec4(local, 1.0f));
        pushBox(view, world, axes, 4 + i, IM_COL32(230, 230, 230, 255), out);
    }
    pushBox(view, transform.position, axes, 3, IM_COL32(230, 230, 230, 255), out);
}

bool ScaleTool::beginDrag(const Handle &handle, const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    if (handle.axis > 2)
        return false;
    
    auto &transform = *raw_transform;
    glm::vec3 dir = transform.rotation * Axes[handle.axis];
    float t;
    if (!rayLineParam(ray, transform.position, dir, t))
        return false;
    
    if (std::abs(t) < 0.01f)
        return false;

    m_dir = dir;
    m_start_scale = transform.scale;
    m_start_t = t;
    m_axis = handle.axis;
    return true;
}

bool ScaleTool::updateDrag(const Ray &ray, ECS::ECS &ecs, ECS::EntityID e)
{
    auto *raw_transform = getTransform(ecs, e);
    if (!raw_transform)
        return false;
    
    auto &transform = *raw_transform;
    float t;
    if (rayLineParam(ray, transform.position, m_dir, t)) {
        float factor = std::max(0.01f, t / m_start_t);
        transform.scale[m_axis] = m_start_scale[m_axis] * factor;
    }
    return true;
}
