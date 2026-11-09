#include "tools/edit/TransformTools.hpp"
#include "tools/viewport/ViewportContext.hpp"
#include "tools/viewport/Gizmo.hpp"
#include "core/DefaultComponents.hpp"

static TransformComponent *getTransform(ECS::ECS &ecs, ECS::EntityID e)
{
    if (!ecs.entityIsActive(e) || !ecs.entityHasComponent<TransformComponent>(e))
        return nullptr;
    return &ecs.entityGetComponent<TransformComponent>(e);
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
