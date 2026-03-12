/**
 * @file Camera.hpp
 * @brief Core camera class with FPS and Orbital modes, lazy-evaluated
 *        view/projection matrices, and configurable settings.
 */

#ifndef ATOM_INPUT_CAMERA_HPP
#define ATOM_INPUT_CAMERA_HPP

#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// CameraMode
// ---------------------------------------------------------------------------

/**
 * @brief Selects the camera control scheme.
 */
enum class CameraMode : uint8_t
{
    FPS = 0, ///< First-person shooter style camera.
    Orbital  ///< Orbit-around-target style camera.
};

// ---------------------------------------------------------------------------
// CameraSettings
// ---------------------------------------------------------------------------

/**
 * @brief Passive configuration struct for camera behaviour parameters.
 *
 * Every field has a sensible default so the struct can be value-initialised.
 */
struct CameraSettings
{
    float moveSpeed = 5.0f;        ///< Base movement speed (units/s).
    float sprintMultiplier = 2.0f; ///< Multiplier applied when sprinting.

    float mouseSensitivity = 0.1f; ///< Mouse look sensitivity.
    bool invertY = false;          ///< Invert vertical mouse axis.

    float minPitchDeg = -89.0f; ///< Minimum pitch in degrees.
    float maxPitchDeg = 89.0f;  ///< Maximum pitch in degrees.

    float orbitDistance = 10.0f;     ///< Default orbital distance.
    float minOrbitDistance = 1.0f;   ///< Minimum orbital distance.
    float maxOrbitDistance = 100.0f; ///< Maximum orbital distance.

    float zoomSpeed = 2.0f; ///< Scroll-wheel zoom speed.
    float panSpeed = 0.01f; ///< Orbital pan speed.
};

// ---------------------------------------------------------------------------
// Camera
// ---------------------------------------------------------------------------

/**
 * @brief General-purpose 3-D camera supporting both FPS and Orbital modes.
 *
 * Matrices and direction vectors are lazily recomputed only when queried
 * after a state change (dirty-flag pattern).
 */
class Camera
{
  public:
    // -- Construction -------------------------------------------------------

    /**
     * @brief Construct a camera with the given control mode.
     * @param mode Initial camera mode (default: FPS).
     *
     * The camera starts at position (0, 0, 5), identity orientation,
     * and a default perspective projection (45 deg FOV, 16:9 aspect,
     * near = 0.1, far = 100).
     */
    explicit Camera(CameraMode mode = CameraMode::FPS);

    // -- Transform ----------------------------------------------------------

    /**
     * @brief Set the world-space position of the camera.
     * @param pos New position.
     */
    void setPosition(const glm::vec3 &pos);

    /**
     * @brief Set the camera orientation from Euler angles in degrees.
     * @param pitchDeg Pitch angle (degrees, positive = look up).
     * @param yawDeg   Yaw angle (degrees, positive = turn right).
     *
     * Pitch is clamped to the range [minPitchDeg, maxPitchDeg] from
     * settings.
     */
    void setEuler(float pitchDeg, float yawDeg);

    /**
     * @brief Set the camera orientation from Euler angles in radians.
     * @param pitch Pitch angle in radians.
     * @param yaw   Yaw angle in radians.
     */
    void setEulerRad(float pitch, float yaw);

    /**
     * @brief Orient the camera to look at a world-space target.
     * @param target The point to look at.
     */
    void lookAt(const glm::vec3 &target);

    // -- Projection ---------------------------------------------------------

    /**
     * @brief Set a full perspective projection.
     * @param fovDeg  Vertical field of view in degrees.
     * @param aspect  Width / height aspect ratio.
     * @param near    Near clipping plane distance.
     * @param far     Far clipping plane distance.
     */
    void setPerspective(float fovDeg, float aspect, float near, float far);

    /**
     * @brief Set the vertical field of view.
     * @param fovDeg Field of view in degrees.
     */
    void setFov(float fovDeg);

    /**
     * @brief Set the aspect ratio (width / height).
     * @param aspect New aspect ratio.
     */
    void setAspect(float aspect);

    // -- Matrices (lazy evaluation) -----------------------------------------

    /**
     * @brief Return the view matrix, recomputing if dirty.
     * @return Const reference to the cached view matrix.
     */
    const glm::mat4 &viewMatrix();

    /**
     * @brief Return the projection matrix, recomputing if dirty.
     * @return Const reference to the cached projection matrix.
     */
    const glm::mat4 &projectionMatrix();

    /**
     * @brief Return the combined view-projection matrix.
     * @return Const reference to the cached VP matrix.
     */
    const glm::mat4 &viewProjectionMatrix();

    // -- Direction vectors (lazy evaluation) --------------------------------

    /**
     * @brief Return the camera's forward direction (negative Z in view).
     * @return Normalised forward vector.
     */
    glm::vec3 forward();

    /**
     * @brief Return the camera's right direction.
     * @return Normalised right vector.
     */
    glm::vec3 right();

    /**
     * @brief Return the camera's up direction.
     * @return Normalised up vector.
     */
    glm::vec3 up();

    // -- Read-only getters --------------------------------------------------

    /**
     * @brief Return the current world-space position.
     * @return Position vector.
     */
    [[nodiscard]] glm::vec3 position() const noexcept;

    /**
     * @brief Return the current pitch in radians.
     * @return Pitch angle (radians).
     */
    [[nodiscard]] float pitch() const noexcept;

    /**
     * @brief Return the current yaw in radians.
     * @return Yaw angle (radians).
     */
    [[nodiscard]] float yaw() const noexcept;

    /**
     * @brief Return the current camera mode.
     * @return Active CameraMode.
     */
    [[nodiscard]] CameraMode mode() const noexcept;

    // -- Settings -----------------------------------------------------------

    /**
     * @brief Access the mutable camera settings.
     * @return Reference to the internal CameraSettings.
     */
    CameraSettings &settings() noexcept;

    /**
     * @brief Access the read-only camera settings.
     * @return Const reference to the internal CameraSettings.
     */
    const CameraSettings &settings() const noexcept;

    // -- Orbital mode -------------------------------------------------------

    /**
     * @brief Set the orbital focal point.
     * @param point World-space target that the camera orbits around.
     */
    void setFocalPoint(const glm::vec3 &point);

    /**
     * @brief Set the distance from the focal point for orbital mode.
     * @param distance Orbit radius (clamped to settings min/max).
     */
    void setOrbitDistance(float distance);

    /**
     * @brief Return the current orbital focal point.
     * @return Focal point vector.
     */
    [[nodiscard]] glm::vec3 focalPoint() const noexcept;

    /**
     * @brief Return the current orbit distance.
     * @return Distance from the focal point.
     */
    [[nodiscard]] float orbitDistance() const noexcept;

    // -- Mode switching -----------------------------------------------------

    /**
     * @brief Switch between FPS and Orbital mode.
     * @param mode The desired camera mode.
     *
     * When switching to Orbital, the focal point is initialised to
     * position + forward * orbitDistance.
     */
    void setMode(CameraMode mode);

    // -- Forced recomputation -----------------------------------------------

    /**
     * @brief Mark all caches dirty and recompute matrices immediately.
     */
    void forceUpdate();

  protected:
    // -- Internal helpers ---------------------------------------------------

    /**
     * @brief Recompute direction vectors from the orientation quaternion.
     */
    void updateVectors();

    /**
     * @brief Recompute view and projection matrices if their dirty flags
     *        are set.
     */
    void updateMatrices();

    /**
     * @brief Build the orientation quaternion from m_pitch / m_yaw.
     *
     * The rotation order is yaw around world Y, then pitch around
     * local X.
     */
    void rebuildOrientation();

    /**
     * @brief Recompute the camera position for orbital mode.
     *
     * position = focalPoint - forward * orbitDistance.
     */
    void updateOrbitalPosition();

    // -- State --------------------------------------------------------------

    glm::vec3 m_position{0.0f, 0.0f, 5.0f};          ///< World position.
    glm::quat m_orientation{1.0f, 0.0f, 0.0f, 0.0f}; ///< Identity quat.

    float m_pitch = 0.0f; ///< Pitch in radians.
    float m_yaw = 0.0f;   ///< Yaw in radians.

    glm::vec3 m_focalPoint{0.0f}; ///< Orbital target.
    float m_orbitDistance = 10.0f;

    glm::vec3 m_forward{0.0f, 0.0f, -1.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};

    glm::mat4 m_viewMatrix{1.0f};
    glm::mat4 m_projMatrix{1.0f};
    glm::mat4 m_vpMatrix{1.0f};

    float m_fovRad = glm::radians(45.0f);
    float m_aspect = 16.0f / 9.0f;
    float m_near = 0.1f;
    float m_far = 100.0f;

    CameraSettings m_settings;
    CameraMode m_mode;

    bool m_viewDirty = true;
    bool m_projDirty = true;
    bool m_vecDirty = true;
};

} // namespace Atom::Input

#endif // ATOM_INPUT_CAMERA_HPP
