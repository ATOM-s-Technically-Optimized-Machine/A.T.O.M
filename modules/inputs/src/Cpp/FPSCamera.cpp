/**
 * @file FPSCamera.cpp
 * @brief Implementation of the Atom::Input::FPSCamera controller.
 */

#include "inputs/FPSCamera.hpp"

#include <cmath>
#include <glm/glm.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// update
// ---------------------------------------------------------------------------

void FPSCamera::update(Camera &camera, const InputState &input, float deltaTime)
{
    update(camera, input, deltaTime, m_defaultBindings);
}

void FPSCamera::update(Camera &camera, const InputState &input, float deltaTime,
                       const FPSBindings &bindings)
{
    processMouseLook(camera, input, bindings);
    processKeyboard(camera, input, bindings, deltaTime);
}

// ---------------------------------------------------------------------------
// processKeyboard
// ---------------------------------------------------------------------------

void FPSCamera::processKeyboard(Camera &camera, const InputState &input,
                                const FPSBindings &bindings, float deltaTime)
{
    glm::vec3 moveDir{0.0f};

    if (input.isKeyDown(bindings.forward))
    {
        moveDir.z += 1.0f;
    }
    if (input.isKeyDown(bindings.backward))
    {
        moveDir.z -= 1.0f;
    }
    if (input.isKeyDown(bindings.right))
    {
        moveDir.x += 1.0f;
    }
    if (input.isKeyDown(bindings.left))
    {
        moveDir.x -= 1.0f;
    }
    if (input.isKeyDown(bindings.up))
    {
        moveDir.y += 1.0f;
    }
    if (input.isKeyDown(bindings.down))
    {
        moveDir.y -= 1.0f;
    }

    float len = glm::length(moveDir);
    if (len < 1e-6f)
    {
        return;
    }
    moveDir /= len;

    float speed = camera.settings().moveSpeed;
    if (input.isKeyDown(bindings.sprint))
    {
        speed *= camera.settings().sprintMultiplier;
    }

    moveLocal(camera, moveDir, speed * deltaTime);
}

// ---------------------------------------------------------------------------
// processMouseLook
// ---------------------------------------------------------------------------

void FPSCamera::processMouseLook(Camera &camera, const InputState &input,
                                 const FPSBindings &bindings)
{
    // If a look button is specified, it must be held.
    if (bindings.lookButton != MouseButton::None && !input.isMouseButtonDown(bindings.lookButton))
    {
        return;
    }

    glm::vec2 delta = input.mouseDelta();
    if (std::abs(delta.x) < 1e-6f && std::abs(delta.y) < 1e-6f)
    {
        return;
    }

    const CameraSettings &s = camera.settings();

    float sensitivity = s.mouseSensitivity * glm::radians(1.0f);

    // Positive yaw = turn right; positive delta.x = mouse moves right.
    // Negate pitch so that moving mouse down looks down.
    float deltaYaw = delta.x * sensitivity;
    float deltaPitch = -delta.y * sensitivity;

    if (s.invertY)
    {
        deltaPitch = -deltaPitch;
    }

    rotate(camera, deltaPitch, deltaYaw);
}

// ---------------------------------------------------------------------------
// move
// ---------------------------------------------------------------------------

void FPSCamera::move(Camera &camera, const glm::vec3 &direction, float distance)
{
    float len = glm::length(direction);
    if (len < 1e-6f)
    {
        return;
    }
    glm::vec3 dir = direction / len;
    camera.setPosition(camera.position() + dir * distance);
}

// ---------------------------------------------------------------------------
// moveLocal
// ---------------------------------------------------------------------------

void FPSCamera::moveLocal(Camera &camera, const glm::vec3 &localDir, float distance)
{
    glm::vec3 fwd = camera.forward();
    glm::vec3 rt = camera.right();
    glm::vec3 upDir = camera.up();

    glm::vec3 worldDir = fwd * localDir.z + rt * localDir.x + upDir * localDir.y;

    float len = glm::length(worldDir);
    if (len < 1e-6f)
    {
        return;
    }
    worldDir /= len;

    camera.setPosition(camera.position() + worldDir * distance);
}

// ---------------------------------------------------------------------------
// rotate
// ---------------------------------------------------------------------------

void FPSCamera::rotate(Camera &camera, float deltaPitchRad, float deltaYawRad)
{
    float newPitch = camera.pitch() + deltaPitchRad;
    float newYaw = camera.yaw() + deltaYawRad;

    // Wrap yaw to [-pi, pi].
    const auto pi = glm::pi<float>();
    newYaw = std::fmod(newYaw + pi, 2.0f * pi);
    if (newYaw < 0.0f)
    {
        newYaw += 2.0f * pi;
    }
    newYaw -= pi;

    // Pitch clamping is handled inside Camera::setEulerRad.
    camera.setEulerRad(newPitch, newYaw);
}

} // namespace Atom::Input
