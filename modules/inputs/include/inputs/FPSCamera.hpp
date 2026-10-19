/**
 * @file FPSCamera.hpp
 * @brief First-person shooter camera controller that drives a Camera
 *        from keyboard and mouse input.
 */

#ifndef ATOM_INPUT_FPSCAMERA_HPP
#define ATOM_INPUT_FPSCAMERA_HPP

#include "inputs/Camera.hpp"
#include "inputs/InputState.hpp"
#include "inputs/KeyCodes.hpp"

#include <glm/glm.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// FPSBindings
// ---------------------------------------------------------------------------

/**
 * @brief Key and mouse button bindings for the FPS camera controller.
 *
 * All fields carry sensible WASD-style defaults.
 */
struct FPSBindings
{
    KeyCode forward = KeyCode::W;     ///< Move forward.
    KeyCode backward = KeyCode::S;    ///< Move backward.
    KeyCode left = KeyCode::A;        ///< Strafe left.
    KeyCode right = KeyCode::D;       ///< Strafe right.
    KeyCode up = KeyCode::Space;      ///< Move up (world Y+).
    KeyCode down = KeyCode::LCtrl;    ///< Move down (world Y-).
    KeyCode sprint = KeyCode::LShift; ///< Sprint modifier.

    /**
     * @brief Mouse button that must be held for mouse-look.
     *
     * Set to MouseButton::None to make mouse-look always active
     * (no button required).
     */
    MouseButton lookButton = MouseButton::None;
};

// ---------------------------------------------------------------------------
// FPSCamera
// ---------------------------------------------------------------------------

/**
 * @brief Stateless FPS-style camera controller.
 *
 * Reads the current InputState each frame and applies movement and
 * rotation to a Camera instance through its public API.
 */
class FPSCamera
{
  public:
    /**
     * @brief Update the camera using the default bindings.
     * @param camera    Camera to manipulate.
     * @param input     Current frame's input state snapshot.
     * @param deltaTime Seconds elapsed since the last frame.
     */
    void update(Camera &camera, const InputState &input, float deltaTime);

    /**
     * @brief Update the camera using custom bindings.
     * @param camera    Camera to manipulate.
     * @param input     Current frame's input state snapshot.
     * @param deltaTime Seconds elapsed since the last frame.
     * @param bindings  Key/button binding overrides.
     */
    void update(Camera &camera, const InputState &input, float deltaTime,
                const FPSBindings &bindings);

    /**
     * @brief Process keyboard movement for the camera.
     * @param camera    Camera to move.
     * @param input     Current input state.
     * @param bindings  Active key bindings.
     * @param deltaTime Frame delta in seconds.
     */
    void processKeyboard(Camera &camera, const InputState &input, const FPSBindings &bindings,
                         float deltaTime);

    /**
     * @brief Process mouse-look rotation for the camera.
     * @param camera   Camera to rotate.
     * @param input    Current input state.
     * @param bindings Active bindings (for look button gating).
     */
    void processMouseLook(Camera &camera, const InputState &input, const FPSBindings &bindings);

    /**
     * @brief Move the camera along a world-space direction.
     * @param camera    Camera to move.
     * @param direction World-space movement direction (will be normalised).
     * @param distance  Distance to move (units).
     */
    void move(Camera &camera, const glm::vec3 &direction, float distance);

    /**
     * @brief Move the camera along a camera-local direction.
     * @param camera   Camera to move.
     * @param localDir Direction in camera-local space (forward = +Z,
     *                 right = +X, up = +Y).
     * @param distance Distance to move (units).
     */
    void moveLocal(Camera &camera, const glm::vec3 &localDir, float distance);

    /**
     * @brief Apply incremental pitch and yaw rotation.
     * @param camera        Camera to rotate.
     * @param deltaPitchRad Pitch delta in radians.
     * @param deltaYawRad   Yaw delta in radians.
     */
    void rotate(Camera &camera, float deltaPitchRad, float deltaYawRad);

  private:
    FPSBindings m_defaultBindings; ///< Bindings used by the single-arg update.
};

} // namespace Atom::Input

#endif // ATOM_INPUT_FPSCAMERA_HPP
