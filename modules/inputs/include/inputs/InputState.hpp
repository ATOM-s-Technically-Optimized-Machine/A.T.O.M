/**
 * @file InputState.hpp
 * @brief Aggregated per-frame input state for keyboard, mouse, modifiers,
 *        and window focus / resize tracking.
 */

#ifndef ATOM_INPUT_INPUTSTATE_HPP
#define ATOM_INPUT_INPUTSTATE_HPP

#include "inputs/Event.hpp"
#include "inputs/KeyCodes.hpp"

#include <array>
#include <cstdint>
#include <glm/ext/vector_int2.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Atom::Input
{

/**
 * @brief Describes the transition state of a key or button within a frame.
 */
enum class KeyState : uint8_t
{
    Up = 0,  ///< Not pressed (was not pressed last frame either).
    Pressed, ///< Just pressed this frame (transition from up to down).
    Held,    ///< Held down (was already pressed last frame).
    Released ///< Just released this frame (transition from down to up).
};

// ---------------------------------------------------------------------------

/**
 * @brief Centralised, frame-coherent snapshot of all input devices.
 *
 * Call @ref beginFrame at the start of each frame, feed every platform
 * event through @ref processEvent, then call @ref endFrame before the
 * game logic reads the state.
 */
class InputState
{
  public:
    // ----- Lifetime --------------------------------------------------------

    /**
     * @brief Construct an InputState with all fields zero-initialised.
     */
    InputState();

    // ----- Frame lifecycle -------------------------------------------------

    /**
     * @brief Prepare the state for a new frame.
     *
     * Copies the current key and button arrays into their "previous"
     * counterparts, computes the delta time (clamped to a maximum of
     * 0.1 s to guard against debugger pauses), and resets per-frame
     * accumulators (mouse delta, scroll accumulator).
     *
     * @param frameTimeMs Wall-clock time of this frame in milliseconds.
     */
    void beginFrame(uint64_t frameTimeMs);

    /**
     * @brief Finalise end-of-frame bookkeeping.
     *
     * Copies the accumulated scroll value so that queries during the
     * next update see the correct per-frame scroll amount.
     */
    void endFrame();

    /**
     * @brief Dispatch a single platform event into the input state.
     *
     * Updates the relevant arrays, accumulators, or flags depending on
     * the event type.
     *
     * @param event The event to process.
     */
    void processEvent(const Event &event);

    /**
     * @brief Reset every field to its default (zero / false) value.
     */
    void reset();

    // ----- Keyboard queries ------------------------------------------------

    /**
     * @brief Check whether a key is currently held down.
     * @param key The key to query.
     * @return @c true if the key is down this frame.
     */
    [[nodiscard]] bool isKeyDown(KeyCode key) const noexcept;

    /**
     * @brief Check whether a key is currently up (not pressed).
     * @param key The key to query.
     * @return @c true if the key is not down this frame.
     */
    [[nodiscard]] bool isKeyUp(KeyCode key) const noexcept;

    /**
     * @brief Check whether a key was pressed this frame (down now, up
     *        last frame).
     * @param key The key to query.
     * @return @c true on the first frame the key is held.
     */
    [[nodiscard]] bool isKeyJustPressed(KeyCode key) const noexcept;

    /**
     * @brief Check whether a key was released this frame (up now, down
     *        last frame).
     * @param key The key to query.
     * @return @c true on the first frame after the key is released.
     */
    [[nodiscard]] bool isKeyJustReleased(KeyCode key) const noexcept;

    /**
     * @brief Return the full transition state of a key.
     * @param key The key to query.
     * @return A @ref KeyState value describing the current / previous
     *         combination.
     */
    [[nodiscard]] KeyState getKeyState(KeyCode key) const noexcept;

    /**
     * @brief Return how long a key has been held, in seconds.
     *
     * Returns 0 if the key is not currently down.
     *
     * @param key The key to query.
     * @return Duration in seconds since the key was first pressed.
     */
    [[nodiscard]] float keyHeldDuration(KeyCode key) const noexcept;

    /**
     * @brief Check whether any key is currently held down.
     * @return @c true if at least one key is down.
     */
    [[nodiscard]] bool isAnyKeyDown() const noexcept;

    /**
     * @brief Check whether any key was just pressed this frame.
     * @return @c true if at least one key transitioned to down.
     */
    [[nodiscard]] bool isAnyKeyJustPressed() const noexcept;

    // ----- Mouse queries ---------------------------------------------------

    /**
     * @brief Return the current mouse cursor position in window
     *        coordinates.
     * @return (x, y) position.
     */
    [[nodiscard]] glm::vec2 mousePosition() const noexcept;

    /**
     * @brief Return the accumulated mouse movement this frame.
     * @return (dx, dy) movement delta.
     */
    [[nodiscard]] glm::vec2 mouseDelta() const noexcept;

    /**
     * @brief Return the per-frame mouse scroll offset.
     * @return (horizontal, vertical) scroll amounts.
     */
    [[nodiscard]] glm::vec2 mouseScroll() const noexcept;

    /**
     * @brief Check whether a mouse button is currently held down.
     * @param btn The button to query.
     * @return @c true if the button is down.
     */
    [[nodiscard]] bool isMouseButtonDown(MouseButton btn) const noexcept;

    /**
     * @brief Check whether a mouse button is currently up.
     * @param btn The button to query.
     * @return @c true if the button is not down.
     */
    [[nodiscard]] bool isMouseButtonUp(MouseButton btn) const noexcept;

    /**
     * @brief Check whether a mouse button was just pressed this frame.
     * @param btn The button to query.
     * @return @c true on the first frame the button is held.
     */
    [[nodiscard]] bool isMouseButtonJustPressed(MouseButton btn) const noexcept;

    /**
     * @brief Check whether a mouse button was just released this frame.
     * @param btn The button to query.
     * @return @c true on the first frame after release.
     */
    [[nodiscard]] bool isMouseButtonJustReleased(MouseButton btn) const noexcept;

    /**
     * @brief Check whether any mouse button is currently held down.
     * @return @c true if at least one button is down.
     */
    [[nodiscard]] bool isAnyMouseButtonDown() const noexcept;

    // ----- Modifier queries ------------------------------------------------

    /**
     * @brief Test whether a modifier flag (or combination) is active.
     * @param mod The modifier flag(s) to test.
     * @return @c true if all bits in @p mod are set.
     */
    [[nodiscard]] bool isModifierActive(Modifier mod) const noexcept;

    /**
     * @brief Convenience: check if either Shift key is held.
     * @return @c true if Shift is active.
     */
    [[nodiscard]] bool isShiftDown() const noexcept;

    /**
     * @brief Convenience: check if either Control key is held.
     * @return @c true if Ctrl is active.
     */
    [[nodiscard]] bool isCtrlDown() const noexcept;

    /**
     * @brief Convenience: check if either Alt key is held.
     * @return @c true if Alt is active.
     */
    [[nodiscard]] bool isAltDown() const noexcept;

    // ----- Movement helpers ------------------------------------------------

    /**
     * @brief Build a normalised movement vector from WASD keys.
     *
     * W = +Z, S = -Z, A = -X, D = +X.  The Y component is always zero.
     * The result is normalised when its length exceeds zero.
     *
     * @return A unit (or zero) direction vector in the XZ plane.
     */
    [[nodiscard]] glm::vec3 getMovementVector() const noexcept;

    /**
     * @brief Return the vertical movement component.
     *
     * Space = +1, Left-Control = -1.
     *
     * @return +1, -1, or 0.
     */
    [[nodiscard]] float getVerticalMovement() const noexcept;

    /**
     * @brief Combine horizontal (WASD) and vertical (Space / LCtrl)
     *        movement into a single normalised 3-D vector.
     * @return A unit (or zero) direction vector.
     */
    [[nodiscard]] glm::vec3 getFullMovementVector() const noexcept;

    // ----- Timing ----------------------------------------------------------

    /**
     * @brief Return the delta time for this frame in seconds.
     * @return Seconds elapsed since the previous frame (clamped).
     */
    [[nodiscard]] float deltaTime() const noexcept;

    /**
     * @brief Return the wall-clock time of the current frame.
     * @return Time in milliseconds.
     */
    [[nodiscard]] uint64_t currentTimeMs() const noexcept;

    // ----- Window ----------------------------------------------------------

    /**
     * @brief Check whether the application window has input focus.
     * @return @c true if the window is focused.
     */
    [[nodiscard]] bool isWindowFocused() const noexcept;

    /**
     * @brief Return the current window dimensions.
     * @return (width, height) in pixels.
     */
    [[nodiscard]] glm::ivec2 windowSize() const noexcept;

  private:
    // ----- Internal helpers ------------------------------------------------

    /**
     * @brief Set or clear modifier bits that correspond to a key.
     * @param key     The key that was pressed or released.
     * @param pressed @c true if the key went down, @c false if released.
     */
    void updateModifiers(KeyCode key, bool pressed);

    // ----- Constants -------------------------------------------------------

    static constexpr auto kKeyCount = static_cast<std::size_t>(KeyCode::Count);
    static constexpr auto kButtonCount = static_cast<std::size_t>(MouseButton::Count);

    // ----- Keyboard --------------------------------------------------------

    std::array<bool, kKeyCount> m_keys{};
    std::array<bool, kKeyCount> m_keysPrev{};
    std::array<uint64_t, kKeyCount> m_keyPressTime{};

    // ----- Mouse -----------------------------------------------------------

    glm::vec2 m_mousePos{0.0f};
    glm::vec2 m_mousePrevPos{0.0f};
    glm::vec2 m_mouseDelta{0.0f};

    glm::vec2 m_scroll{0.0f};
    glm::vec2 m_scrollAccum{0.0f};

    std::array<bool, kButtonCount> m_buttons{};
    std::array<bool, kButtonCount> m_buttonsPrev{};

    // ----- Modifiers -------------------------------------------------------

    uint16_t m_modifiers = 0;

    // ----- Timing ----------------------------------------------------------

    float m_deltaTime = 0.0f;
    uint64_t m_currentTimeMs = 0;
    uint64_t m_prevTimeMs = 0;

    // ----- Window ----------------------------------------------------------

    bool m_windowFocused = true;
    int32_t m_windowWidth = 0;
    int32_t m_windowHeight = 0;
};

} // namespace Atom::Input

#endif // ATOM_INPUT_INPUTSTATE_HPP
