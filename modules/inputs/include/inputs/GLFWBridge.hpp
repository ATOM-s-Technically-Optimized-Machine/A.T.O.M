/**
 * @file GLFWBridge.hpp
 * @brief GLFW-to-EventQueue bridge that translates GLFW callbacks into
 *        Atom::Input::Event objects.
 */

#ifndef ATOM_INPUT_GLFWBRIDGE_HPP
#define ATOM_INPUT_GLFWBRIDGE_HPP

#ifdef ATOM_INPUT_HAS_GLFW

#define GLFW_INCLUDE_NONE
#include "inputs/Event.hpp"
#include "inputs/KeyCodes.hpp"

#include <GLFW/glfw3.h>
#include <cstdint>

namespace Atom::Input
{

/**
 * @brief Connects a GLFW window to an EventQueue.
 *
 * On construction, the bridge installs all necessary GLFW callbacks on the
 * given window. Every callback translates its parameters into an Event and
 * pushes it to the EventQueue. On destruction, all callbacks are removed.
 *
 * The class is non-copyable but movable.
 */
class GLFWBridge
{
  public:
    // ----- Lifetime --------------------------------------------------------

    /**
     * @brief Construct a bridge and install GLFW callbacks.
     *
     * Stores the window and queue pointers, calls
     * @c glfwSetWindowUserPointer to associate @c this with the window,
     * and installs all seven GLFW callbacks.
     *
     * @param window The GLFW window to listen on. Must not be null.
     * @param queue  The event queue that will receive translated events.
     */
    GLFWBridge(GLFWwindow *window, EventQueue &queue);

    /**
     * @brief Destroy the bridge and reset all GLFW callbacks to nullptr.
     */
    ~GLFWBridge();

    GLFWBridge(const GLFWBridge &) = delete;
    GLFWBridge &operator=(const GLFWBridge &) = delete;

    GLFWBridge(GLFWBridge &&) noexcept = default;
    GLFWBridge &operator=(GLFWBridge &&) noexcept = default;

    // ----- Configuration ---------------------------------------------------

    /**
     * @brief Enable or disable raw mouse motion input.
     *
     * Raw mouse motion bypasses the OS pointer acceleration. This is
     * only effective when the cursor is captured (disabled).
     *
     * @param enabled True to enable raw mouse motion.
     */
    void setRawMouseMotion(bool enabled);

    /**
     * @brief Capture or release the mouse cursor.
     *
     * When captured, the cursor is hidden and locked to the window
     * (GLFW_CURSOR_DISABLED). When released, it returns to normal
     * (GLFW_CURSOR_NORMAL).
     *
     * @param captured True to capture, false to release.
     */
    void setCursorCaptured(bool captured);

    // ----- Convenience -----------------------------------------------------

    /**
     * @brief Poll for pending GLFW events.
     *
     * Convenience wrapper around @c glfwPollEvents().
     */
    void pollEvents();

  private:
    // ----- Data members ----------------------------------------------------

    GLFWwindow *m_window; ///< The GLFW window this bridge is attached to.
    EventQueue *m_queue;  ///< Destination queue for translated events.

    double m_lastMouseX = 0.0; ///< Previous cursor X for delta computation.
    double m_lastMouseY = 0.0; ///< Previous cursor Y for delta computation.
    bool m_firstMouse = true;  ///< True until the first cursor event.

    // ----- Static GLFW callbacks -------------------------------------------

    /**
     * @brief GLFW key callback.
     */
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    /**
     * @brief GLFW mouse button callback.
     */
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    /**
     * @brief GLFW cursor position callback.
     */
    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    /**
     * @brief GLFW scroll callback.
     */
    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    /**
     * @brief GLFW window size callback.
     */
    static void windowSizeCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief GLFW window focus callback.
     */
    static void windowFocusCallback(GLFWwindow *window, int focused);

    /**
     * @brief GLFW window close callback.
     */
    static void windowCloseCallback(GLFWwindow *window);

    // ----- Static mapping helpers ------------------------------------------

    /**
     * @brief Map a GLFW key constant to a KeyCode.
     * @param glfwKey The GLFW_KEY_* constant.
     * @return The corresponding KeyCode, or KeyCode::Unknown.
     */
    static KeyCode mapGLFWKey(int glfwKey);

    /**
     * @brief Map a GLFW mouse button constant to a MouseButton.
     * @param glfwButton The GLFW_MOUSE_BUTTON_* constant.
     * @return The corresponding MouseButton.
     */
    static MouseButton mapGLFWMouseButton(int glfwButton);

    /**
     * @brief Map GLFW modifier flags to Modifier bit flags.
     * @param glfwMods The GLFW_MOD_* bitmask.
     * @return A uint16_t containing the combined Modifier bits.
     */
    static uint16_t mapGLFWMods(int glfwMods);

    /**
     * @brief Return the current time in milliseconds from GLFW.
     * @return Milliseconds since GLFW initialisation.
     */
    static uint64_t currentTimeMs();
};

} // namespace Atom::Input

#endif // ATOM_INPUT_HAS_GLFW

#endif // ATOM_INPUT_GLFWBRIDGE_HPP
