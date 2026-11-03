#ifndef PICKING_HPP_
#define PICKING_HPP_

#include <cfloat>
#include <vector>
#include "core/ECS/ECS.hpp"
#include "core/MathUtils.hpp"
#include "core/DefaultComponents.hpp"

// Returns true if entity has mesh component and fills bmin and bmax in place
// Returns false without touching bmin and bmax otherwise
inline bool getEntityBounds(ECS::ECS &ecs, ECS::EntityID e, glm::vec3 &bmin, glm::vec3 &bmax)
{
    if (ecs.entityHasComponent<MeshHandleComponent>(e)) {
        auto &mesh = ecs.entityGetComponent<MeshHandleComponent>(e);
        bmin = mesh.boundsMin;
        bmax = mesh.boundsMax;
        return true;
    }
    return false;
}

inline bool rayHitsEntity(ECS::ECS &ecs, ECS::EntityID e, const Ray &ray, float &t_hit)
{
    auto &transform = ecs.entityGetComponent<TransformComponent>(e);
    glm::vec3 bmin(-0.5f), bmax(0.5f);

    getEntityBounds(ecs, e, bmin, bmax);

    // Move the ray into the entity's local space (where its box is axis aligned)
    // The direction is NOT normalized so t stays a world-space distance
    glm::mat4 inv = TransformComponent::toLocal(transform);
    Ray local(transformPoint(inv, ray.origin), transformDirection(inv, ray.direction));

    return rayAABB(local, bmin, bmax, t_hit);
}

inline ECS::EntityID pickEntity(ECS::ECS &ecs, const Ray &ray)
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

#endif /* PICKING_HPP_ */