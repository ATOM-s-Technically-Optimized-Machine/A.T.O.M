/**
 * @file OrbitalCamera.hpp
 * @brief Orbit-around-target camera controller driven by mouse input.
 */

#ifndef ATOM_INPUT_ORBITALCAMERA_HPP
#define ATOM_INPUT_ORBITALCAMERA_HPP

#include "inputs/Camera.hpp"
#include "inputs/InputState.hpp"
#include "inputs/KeyCodes.hpp"

#include <glm/glm.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// OrbitalBindings
// ---------------------------------------------------------------------------

/**
 * @brief Mouse button and key bindings for the orbital camera controller.
 */
struct OrbitalBindings
{
    MouseButton orbitButton = MouseButton::Left; ///< Hold to orbit.
    MouseButton panButton = MouseButton::Middle; ///< Hold to pan.
    KeyCode resetKey = KeyCode::R;               ///< Press to reset view.
};

// ---------------------------------------------------------------------------
// OrbitalCamera
// ---------------------------------------------------------------------------

/**
 * @brief Stateless orbital camera controller.
 *
 * Reads the current InputState each frame and applies orbit, pan, and
 * zoom operations to a Camera instance through its public API.
 */
class OrbitalCamera
{
  public:
    /**
     * @brief Update the camera using the default bindings.
     * @param camera    Camera to manipulate (should be in Orbital mode).
     * @param input     Current frame's input state snapshot.
     * @param deltaTime Seconds elapsed since the last frame.
     */
    void update(Camera &camera, const InputState &input, float deltaTime);

    /**
     * @brief Update the camera using custom bindings.
     * @param camera    Camera to manipulate.
     * @param input     Current frame's input state snapshot.
     * @param deltaTime Seconds elapsed since the last frame.
     * @param bindings  Button/key binding overrides.
     */
    void update(Camera &camera, const InputState &input, float deltaTime,
                const OrbitalBindings &bindings);

    /**
     * @brief Process orbital rotation from mouse drag.
     * @param camera   Camera to orbit.
     * @param input    Current input state.
     * @param bindings Active bindings.
     */
    void processOrbit(Camera &camera, const InputState &input, const OrbitalBindings &bindings);

    /**
     * @brief Process panning (focal-point translation) from mouse drag.
     * @param camera   Camera to pan.
     * @param input    Current input state.
     * @param bindings Active bindings.
     */
    void processPan(Camera &camera, const InputState &input, const OrbitalBindings &bindings);

    /**
     * @brief Process zoom from mouse scroll.
     * @param camera Camera to zoom.
     * @param input  Current input state.
     */
    void processZoom(Camera &camera, const InputState &input);

    /**
     * @brief Apply an incremental orbit rotation.
     * @param camera            Camera to rotate.
     * @param deltaAzimuthRad   Horizontal rotation delta (radians).
     * @param deltaElevationRad Vertical rotation delta (radians).
     */
    void orbit(Camera &camera, float deltaAzimuthRad, float deltaElevationRad);

    /**
     * @brief Translate the focal point in camera-local right/up plane.
     * @param camera Camera to pan.
     * @param delta  Pan amount (x = right, y = up) in world units.
     */
    void pan(Camera &camera, const glm::vec2 &delta);

    /**
     * @brief Adjust the orbital distance.
     * @param camera        Camera to zoom.
     * @param deltaDistance  Distance change (positive = zoom out).
     */
    void zoom(Camera &camera, float deltaDistance);

    /**
     * @brief Reset the orbital camera to a known state.
     * @param camera     Camera to reset.
     * @param focalPoint New focal point.
     * @param distance   Orbit distance.
     * @param pitchDeg   Initial pitch in degrees.
     * @param yawDeg     Initial yaw in degrees.
     */
    void reset(Camera &camera, const glm::vec3 &focalPoint, float distance, float pitchDeg,
               float yawDeg);

  private:
    OrbitalBindings m_defaultBindings; ///< Bindings used by single-arg update.
};

} // namespace Atom::Input

#endif // ATOM_INPUT_ORBITALCAMERA_HPP
