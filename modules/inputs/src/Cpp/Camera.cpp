/**
 * @file Camera.cpp
 * @brief Implementation of the Atom::Input::Camera class.
 */

#include "inputs/Camera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/gtc/quaternion.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Camera::Camera(CameraMode mode) : m_mode(mode)
{
    rebuildOrientation();
    updateVectors();
    updateMatrices();
}

// ---------------------------------------------------------------------------
// Transform
// ---------------------------------------------------------------------------

void Camera::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
    m_viewDirty = true;
}

void Camera::setEuler(float pitchDeg, float yawDeg)
{
    pitchDeg = std::clamp(pitchDeg, m_settings.minPitchDeg, m_settings.maxPitchDeg);

    m_pitch = glm::radians(pitchDeg);
    m_yaw = glm::radians(yawDeg);

    rebuildOrientation();

    m_vecDirty = true;
    m_viewDirty = true;
}

void Camera::setEulerRad(float pitch, float yaw)
{
    float minRad = glm::radians(m_settings.minPitchDeg);
    float maxRad = glm::radians(m_settings.maxPitchDeg);

    m_pitch = std::clamp(pitch, minRad, maxRad);
    m_yaw = yaw;

    rebuildOrientation();

    m_vecDirty = true;
    m_viewDirty = true;
}

void Camera::lookAt(const glm::vec3 &target)
{
    glm::vec3 dir = glm::normalize(target - m_position);

    m_pitch = std::asin(dir.y);
    m_yaw = std::atan2(dir.x, -dir.z);

    rebuildOrientation();

    m_vecDirty = true;
    m_viewDirty = true;
}

// ---------------------------------------------------------------------------
// Projection
// ---------------------------------------------------------------------------

void Camera::setPerspective(float fovDeg, float aspect, float near, float far)
{
    m_fovRad = glm::radians(fovDeg);
    m_aspect = aspect;
    m_near = near;
    m_far = far;
    m_projDirty = true;
}

void Camera::setFov(float fovDeg)
{
    m_fovRad = glm::radians(fovDeg);
    m_projDirty = true;
}

void Camera::setAspect(float aspect)
{
    m_aspect = aspect;
    m_projDirty = true;
}

// ---------------------------------------------------------------------------
// Matrices (lazy)
// ---------------------------------------------------------------------------

const glm::mat4 &Camera::viewMatrix()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    if (m_viewDirty || m_projDirty)
    {
        updateMatrices();
    }
    return m_viewMatrix;
}

const glm::mat4 &Camera::projectionMatrix()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    if (m_viewDirty || m_projDirty)
    {
        updateMatrices();
    }
    return m_projMatrix;
}

const glm::mat4 &Camera::viewProjectionMatrix()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    if (m_viewDirty || m_projDirty)
    {
        updateMatrices();
    }
    return m_vpMatrix;
}

// ---------------------------------------------------------------------------
// Direction vectors (lazy)
// ---------------------------------------------------------------------------

glm::vec3 Camera::forward()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    return m_forward;
}

glm::vec3 Camera::right()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    return m_right;
}

glm::vec3 Camera::up()
{
    if (m_vecDirty)
    {
        updateVectors();
    }
    return m_up;
}

// ---------------------------------------------------------------------------
// Read-only getters
// ---------------------------------------------------------------------------

glm::vec3 Camera::position() const noexcept
{
    return m_position;
}

float Camera::pitch() const noexcept
{
    return m_pitch;
}

float Camera::yaw() const noexcept
{
    return m_yaw;
}

CameraMode Camera::mode() const noexcept
{
    return m_mode;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

CameraSettings &Camera::settings() noexcept
{
    return m_settings;
}

const CameraSettings &Camera::settings() const noexcept
{
    return m_settings;
}

// ---------------------------------------------------------------------------
// Orbital
// ---------------------------------------------------------------------------

void Camera::setFocalPoint(const glm::vec3 &point)
{
    m_focalPoint = point;
    m_viewDirty = true;
}

void Camera::setOrbitDistance(float distance)
{
    m_orbitDistance =
        std::clamp(distance, m_settings.minOrbitDistance, m_settings.maxOrbitDistance);
    m_viewDirty = true;
}

glm::vec3 Camera::focalPoint() const noexcept
{
    return m_focalPoint;
}

float Camera::orbitDistance() const noexcept
{
    return m_orbitDistance;
}

// ---------------------------------------------------------------------------
// Mode switching
// ---------------------------------------------------------------------------

void Camera::setMode(CameraMode mode)
{
    if (mode == m_mode)
    {
        return;
    }

    if (mode == CameraMode::Orbital)
    {
        // Ensure vectors are up-to-date before computing focal point.
        if (m_vecDirty)
        {
            updateVectors();
        }
        m_focalPoint = m_position + m_forward * m_orbitDistance;
    }

    m_mode = mode;
    m_viewDirty = true;
}

// ---------------------------------------------------------------------------
// Forced recomputation
// ---------------------------------------------------------------------------

void Camera::forceUpdate()
{
    m_vecDirty = true;
    m_projDirty = true;
    m_viewDirty = true;

    updateVectors();
    updateMatrices();
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void Camera::rebuildOrientation()
{
    // Negate yaw so that positive yaw = turn right (clockwise when viewed
    // from above), which matches the standard FPS camera convention.
    m_orientation = glm::angleAxis(-m_yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
                    glm::angleAxis(m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    m_orientation = glm::normalize(m_orientation);
}

void Camera::updateVectors()
{
    m_forward = m_orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    m_right = m_orientation * glm::vec3(1.0f, 0.0f, 0.0f);
    m_up = m_orientation * glm::vec3(0.0f, 1.0f, 0.0f);

    m_vecDirty = false;
}

void Camera::updateMatrices()
{
    if (m_projDirty)
    {
        m_projMatrix = glm::perspective(m_fovRad, m_aspect, m_near, m_far);
        m_projDirty = false;
    }

    if (m_viewDirty)
    {
        if (m_mode == CameraMode::Orbital)
        {
            updateOrbitalPosition();
        }
        m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
        m_viewDirty = false;
    }

    m_vpMatrix = m_projMatrix * m_viewMatrix;
}

void Camera::updateOrbitalPosition()
{
    m_position = m_focalPoint - m_forward * m_orbitDistance;
}

} // namespace Atom::Input
