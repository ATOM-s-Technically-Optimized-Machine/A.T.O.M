/*
** EPITECH PROJECT, 2026
** main
** File description:
** DefaultComponents
*/

#include "core/DefaultComponents.hpp"
#include "core/ECS/Includes.hpp"

glm::mat4 TransformComponent::toWorld(const TransformComponent &t)
{
    glm::mat4 m = glm::translate(glm::mat4(1.0f), t.position);

    m *= glm::mat4_cast(t.rotation);
    m = glm::scale(m, t.scale);
    return m;
}

glm::mat4 TransformComponent::toLocal(const TransformComponent &t)
{
    return glm::inverse(TransformComponent::toWorld(t));
}
