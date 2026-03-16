/**
 * @file ECSIntegration.hpp
 * @brief Plug-and-play ECS components and systems for integrating the
 *        ATOM input module into an ECS-driven game loop.
 */

#ifndef ATOM_INPUT_ECSINTEGRATION_HPP
#define ATOM_INPUT_ECSINTEGRATION_HPP

#include "inputs/Camera.hpp"
#include "inputs/FPSCamera.hpp"
#include "inputs/InputSystem.hpp"
#include "inputs/OrbitalCamera.hpp"

#include <core/ECS/ECS.hpp>
#include <cstdint>
#include <memory>

#ifdef ATOM_INPUT_HAS_GLFW
#include "inputs/GLFWBridge.hpp"
#endif

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// ControlMode
// ---------------------------------------------------------------------------

/**
 * @brief Selects which camera controller drives a CameraComponent.
 */
enum class ControlMode : uint8_t
{
    FPS = 0, ///< First-person shooter style camera.
    Orbital  ///< Orbit-around-target style camera.
};

// ---------------------------------------------------------------------------
// CameraComponent
// ---------------------------------------------------------------------------

/**
 * @brief ECS component attached to every entity that represents a camera.
 *
 * Holds the Camera instance together with its control mode, per-mode
 * key bindings, an enabled flag, and a normalised viewport rectangle.
 */
struct CameraComponent
{
    Camera camera;                       ///< The camera instance.
    ControlMode mode = ControlMode::FPS; ///< Active control scheme.
    FPSBindings fpsBindings;             ///< FPS-mode key bindings.
    OrbitalBindings orbitalBindings;     ///< Orbital-mode bindings.
    bool enabled = true;                 ///< Process this camera?

    /**
     * @brief Normalised viewport rectangle (0..1 range).
     */
    struct ViewPort
    {
        float x = 0.0f;      ///< Left edge (normalised).
        float y = 0.0f;      ///< Top edge (normalised).
        float width = 1.0f;  ///< Width (normalised).
        float height = 1.0f; ///< Height (normalised).
    };

    ViewPort viewport; ///< Viewport for this camera.
};

// ---------------------------------------------------------------------------
// InputECSSystem
// ---------------------------------------------------------------------------

/**
 * @brief Processes input events for InputSystem each frame.
 *
 * Register this system in the ECS
 */
class InputECSSystem : public ECS::ISystem
{
  public:
    /**
     * @brief Calls InputSystem.update();
     * @param ecs    The ECS instance.
     * @param thisID The system's own ID.
     * @param msecs  Milliseconds elapsed since the last tick.
     */
    void Update(ECS::ECS &ecs, ECS::SystemID thisID, uint32_t msecs) override;

    std::shared_ptr<InputSystem> inputSystem() const;

#ifdef ATOM_INPUT_HAS_GLFW
    void bindGLFW(GLFWwindow *window);
#endif
  private:
    std::shared_ptr<InputSystem> m_input_system = std::make_shared<InputSystem>(); ///< Input system tied to the ECS system instance.
#ifdef ATOM_INPUT_HAS_GLFW
    /**
     * @brief Optional GLFW bridge (nullptr when no GLFW window is bound).
     */
    std::unique_ptr<GLFWBridge> m_bridge;
#endif
};

// ---------------------------------------------------------------------------
// CameraECSSystem
// ---------------------------------------------------------------------------

/**
 * @brief Updates all CameraComponent entities using the input state.
 *
 * Register this system **after** InputECSSystem so that input state is
 * fresh when cameras are updated.
 */
class CameraECSSystem : public ECS::ISystem
{
  public:
    CameraECSSystem(std::shared_ptr<InputSystem> input);
    /**
     * @brief Update every enabled CameraComponent using the current
     *        InputState.
     * @param ecs    The ECS instance.
     * @param thisID The system's own ID.
     * @param msecs  Milliseconds elapsed since the last tick.
     */
    void Update(ECS::ECS &ecs, ECS::SystemID thisID, uint32_t msecs) override;

  private:
    FPSCamera m_fps;         ///< Shared FPS controller instance.
    OrbitalCamera m_orbital; ///< Shared Orbital controller instance.
    std::shared_ptr<InputSystem> m_input;
};

} // namespace Atom::Input

#endif // ATOM_INPUT_ECSINTEGRATION_HPP
