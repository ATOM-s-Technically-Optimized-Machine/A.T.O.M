/**
 * @file OrbitalCamera.cpp
 * @brief Implementation of the Atom::Input::OrbitalCamera controller.
 */

#include "inputs/OrbitalCamera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// update
// ---------------------------------------------------------------------------

void OrbitalCamera::update(Camera &camera, const InputState &input, float deltaTime)
{
    update(camera, input, deltaTime, m_defaultBindings);
}

void OrbitalCamera::update(Camera &camera, const InputState &input, float /*deltaTime*/,
                           const OrbitalBindings &bindings)
{
    // Reset takes priority.
    if (input.isKeyJustPressed(bindings.resetKey))
    {
        reset(camera, glm::vec3(0.0f), 10.0f, 0.0f, 0.0f);
        return;
    }

    // Zoom is always processed (scroll wheel).
    processZoom(camera, input);

    // Pan takes priority over orbit when both buttons are held.
    if (input.isMouseButtonDown(bindings.panButton))
    {
        processPan(camera, input, bindings);
    }
    else if (input.isMouseButtonDown(bindings.orbitButton))
    {
        processOrbit(camera, input, bindings);
    }
}

// ---------------------------------------------------------------------------
// processOrbit
// ---------------------------------------------------------------------------

void OrbitalCamera::processOrbit(Camera &camera, const InputState &input,
                                 const OrbitalBindings & /*bindings*/)
{
    glm::vec2 delta = input.mouseDelta();
    if (std::abs(delta.x) < 1e-6f && std::abs(delta.y) < 1e-6f)
    {
        return;
    }

    const CameraSettings &s = camera.settings();
    float sensitivity = s.mouseSensitivity * glm::radians(1.0f);

    float azimuth = delta.x * sensitivity;
    float elevation = -delta.y * sensitivity;

    orbit(camera, azimuth, elevation);
}

// ---------------------------------------------------------------------------
// processPan
// ---------------------------------------------------------------------------

void OrbitalCamera::processPan(Camera &camera, const InputState &input,
                               const OrbitalBindings & /*bindings*/)
{
    glm::vec2 delta = input.mouseDelta();
    if (std::abs(delta.x) < 1e-6f && std::abs(delta.y) < 1e-6f)
    {
        return;
    }

    const CameraSettings &s = camera.settings();

    float scale = s.panSpeed * camera.orbitDistance();
    glm::vec2 panDelta(-delta.x * scale, delta.y * scale);

    pan(camera, panDelta);
}

// ---------------------------------------------------------------------------
// processZoom
// ---------------------------------------------------------------------------

void OrbitalCamera::processZoom(Camera &camera, const InputState &input)
{
    glm::vec2 scroll = input.mouseScroll();
    if (std::abs(scroll.y) < 1e-6f)
    {
        return;
    }

    const CameraSettings &s = camera.settings();
    float dist = camera.orbitDistance();
    float delta = -scroll.y * s.zoomSpeed * (dist * 0.1f);

    zoom(camera, delta);
}

// ---------------------------------------------------------------------------
// orbit
// ---------------------------------------------------------------------------

void OrbitalCamera::orbit(Camera &camera, float deltaAzimuthRad, float deltaElevationRad)
{
    float newPitch = camera.pitch() + deltaElevationRad;
    float newYaw = camera.yaw() + deltaAzimuthRad;

    // Pitch clamping is handled inside Camera::setEulerRad.
    camera.setEulerRad(newPitch, newYaw);
    camera.forceUpdate();
}

// ---------------------------------------------------------------------------
// pan
// ---------------------------------------------------------------------------

void OrbitalCamera::pan(Camera &camera, const glm::vec2 &delta)
{
    glm::vec3 offset = camera.right() * delta.x + camera.up() * delta.y;

    camera.setFocalPoint(camera.focalPoint() + offset);
    camera.forceUpdate();
}

// ---------------------------------------------------------------------------
// zoom
// ---------------------------------------------------------------------------

void OrbitalCamera::zoom(Camera &camera, float deltaDistance)
{
    const CameraSettings &s = camera.settings();
    float newDist =
        std::clamp(camera.orbitDistance() + deltaDistance, s.minOrbitDistance, s.maxOrbitDistance);

    camera.setOrbitDistance(newDist);
}

// ---------------------------------------------------------------------------
// reset
// ---------------------------------------------------------------------------

void OrbitalCamera::reset(Camera &camera, const glm::vec3 &focalPoint, float distance,
                          float pitchDeg, float yawDeg)
{
    const CameraSettings &s = camera.settings();

    camera.setFocalPoint(focalPoint);
    camera.setOrbitDistance(std::clamp(distance, s.minOrbitDistance, s.maxOrbitDistance));
    camera.setEuler(pitchDeg, yawDeg);
    camera.forceUpdate();
}

} // namespace Atom::Input
