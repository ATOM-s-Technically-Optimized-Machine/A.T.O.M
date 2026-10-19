/**
 * @file InputSystem.hpp
 * @brief RAII orchestrator that ties together EventQueue, EventDispatcher,
 *        and InputState into a single, easy-to-use input facade.
 */

#ifndef ATOM_INPUT_INPUTSYSTEM_HPP
#define ATOM_INPUT_INPUTSYSTEM_HPP

#include "inputs/Event.hpp"
#include "inputs/InputState.hpp"

#include <cstdint>

namespace Atom::Input
{

/**
 * @brief High-level input system that owns the event queue, dispatcher,
 *        and aggregated input state.
 *
 * Typical per-frame usage:
 * @code
 *     inputSystem.update(currentTimeMs);
 *     // -- or, for finer control --
 *     inputSystem.beginFrame(currentTimeMs);
 *     inputSystem.processEvents();
 *     inputSystem.endFrame();
 * @endcode
 *
 * The class is non-copiable but movable.
 */
class InputSystem
{
  public:
    // ----- Lifetime --------------------------------------------------------

    /**
     * @brief Construct the input system.
     *
     * All sub-objects (queue, dispatcher, state) self-initialise via their
     * own default constructors.
     */
    InputSystem();

    /**
     * @brief Destroy the input system.
     */
    ~InputSystem();

    /// Non-copiable.
    InputSystem(const InputSystem &) = delete;

    /// Non-copiable.
    InputSystem &operator=(const InputSystem &) = delete;

    /// Movable.
    InputSystem(InputSystem &&) noexcept = default;

    /// Movable.
    InputSystem &operator=(InputSystem &&) noexcept = default;

    // ----- Frame lifecycle -------------------------------------------------

    /**
     * @brief Prepare the input state for a new frame.
     *
     * Forwards to InputState::beginFrame().
     *
     * @param frameTimeMs Wall-clock time of this frame in milliseconds.
     */
    void beginFrame(uint64_t frameTimeMs);

    /**
     * @brief Drain the event queue, dispatch to listeners, and feed the
     *        input state.
     *
     * Each event is first dispatched to registered EventDispatcher
     * listeners, then forwarded to InputState::processEvent().
     * WindowClose events additionally set the internal quit flag.
     */
    void processEvents();

    /**
     * @brief Finalise end-of-frame bookkeeping.
     *
     * Forwards to InputState::endFrame().
     */
    void endFrame();

    /**
     * @brief Convenience: beginFrame + processEvents + endFrame.
     *
     * @param frameTimeMs Wall-clock time of this frame in milliseconds.
     */
    void update(uint64_t frameTimeMs);

    // ----- Queue access ----------------------------------------------------

    /**
     * @brief Return a reference to the internal event queue.
     *
     * Platform bridges (e.g. GLFWBridge) push events into this queue.
     *
     * @return Mutable reference to the EventQueue.
     */
    EventQueue &eventQueue() noexcept;

    // ----- Listener management ---------------------------------------------

    /**
     * @brief Register a callback for a specific event type.
     *
     * Forwards to EventDispatcher::addListener().
     *
     * @param type     The event type to listen for.
     * @param callback The function to call when a matching event arrives.
     * @return A unique listener identifier for later removal.
     */
    EventDispatcher::ListenerID addListener(EventType type,
                                            EventDispatcher::EventCallback callback);

    /**
     * @brief Remove a previously registered listener.
     *
     * Forwards to EventDispatcher::removeListener().
     *
     * @param id The identifier returned by addListener().
     */
    void removeListener(EventDispatcher::ListenerID id);

    // ----- State queries ---------------------------------------------------

    /**
     * @brief Return a read-only reference to the aggregated input state.
     * @return Const reference to the InputState.
     */
    const InputState &state() const noexcept;

    /**
     * @brief Return the delta time for this frame in seconds.
     *
     * Convenience shorthand for state().deltaTime().
     *
     * @return Seconds elapsed since the previous frame (clamped).
     */
    float deltaTime() const noexcept;

    /**
     * @brief Check whether a window-close event has been received.
     * @return @c true if the application should quit.
     */
    bool shouldQuit() const noexcept;

    // ----- Mouse capture ---------------------------------------------------

    /**
     * @brief Set the mouse-captured flag.
     *
     * This is an advisory flag that game logic can query; the actual
     * cursor grab/release must be performed by the platform layer.
     *
     * @param captured @c true to indicate the mouse is captured.
     */
    void captureMouse(bool captured);

    /**
     * @brief Check whether the mouse is currently captured.
     * @return @c true if the mouse-captured flag is set.
     */
    bool isMouseCaptured() const noexcept;

  private:
    EventQueue m_queue;
    EventDispatcher m_dispatcher;
    InputState m_state;
    bool m_mouseCaptured = false;
    bool m_shouldQuit = false;
};

} // namespace Atom::Input

#endif // ATOM_INPUT_INPUTSYSTEM_HPP
