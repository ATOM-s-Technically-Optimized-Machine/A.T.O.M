/**
 * @file GLFWBridge.cpp
 * @brief Implementation of the Atom::Input::GLFWBridge class.
 */

#ifdef ATOM_INPUT_HAS_GLFW

#include "inputs/GLFWBridge.hpp"

namespace Atom::Input
{

// =========================================================================
// Lifetime
// =========================================================================

GLFWBridge::GLFWBridge(GLFWwindow *window, EventQueue &queue) : m_window(window), m_queue(&queue)
{
    glfwSetWindowUserPointer(m_window, this);

    // Install all seven callbacks.
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetWindowSizeCallback(m_window, windowSizeCallback);
    glfwSetWindowFocusCallback(m_window, windowFocusCallback);
    glfwSetWindowCloseCallback(m_window, windowCloseCallback);

    // Initialise last mouse position from the current cursor state.
    glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
    m_firstMouse = true;
}

GLFWBridge::~GLFWBridge()
{
    if (!m_window)
    {
        return;
    }

    // Remove all callbacks.
    glfwSetKeyCallback(m_window, nullptr);
    glfwSetMouseButtonCallback(m_window, nullptr);
    glfwSetCursorPosCallback(m_window, nullptr);
    glfwSetScrollCallback(m_window, nullptr);
    glfwSetWindowSizeCallback(m_window, nullptr);
    glfwSetWindowFocusCallback(m_window, nullptr);
    glfwSetWindowCloseCallback(m_window, nullptr);

    // Clear the user pointer.
    glfwSetWindowUserPointer(m_window, nullptr);
}

// =========================================================================
// Configuration
// =========================================================================

void GLFWBridge::setRawMouseMotion(bool enabled)
{
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, enabled ? GLFW_TRUE : GLFW_FALSE);
    }
}

void GLFWBridge::setCursorCaptured(bool captured)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
}

// =========================================================================
// Convenience
// =========================================================================

void GLFWBridge::pollEvents()
{
    glfwPollEvents();
}

// =========================================================================
// GLFW callbacks
// =========================================================================

void GLFWBridge::keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int mods)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();

    switch (action)
    {
    case GLFW_PRESS:
        event.type = EventType::KeyDown;
        event.key.isRepeat = false;
        break;
    case GLFW_RELEASE:
        event.type = EventType::KeyUp;
        event.key.isRepeat = false;
        break;
    case GLFW_REPEAT:
        event.type = EventType::KeyRepeat;
        event.key.isRepeat = true;
        break;
    default:
        return;
    }

    event.key.key = mapGLFWKey(key);
    event.key.modifiers = mapGLFWMods(mods);

    bridge->m_queue->push(event);
}

void GLFWBridge::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = (action == GLFW_PRESS) ? EventType::MouseButtonDown : EventType::MouseButtonUp;

    event.mouseButton.button = mapGLFWMouseButton(button);
    event.mouseButton.clicks = 1;

    // Record cursor position at the time of the click.
    double cx = 0.0;
    double cy = 0.0;
    glfwGetCursorPos(window, &cx, &cy);
    event.mouseButton.x = static_cast<int32_t>(cx);
    event.mouseButton.y = static_cast<int32_t>(cy);

    (void)mods; // Modifier state is tracked separately by InputState.

    bridge->m_queue->push(event);
}

void GLFWBridge::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    // On the very first cursor event, suppress the delta to avoid a
    // large jump from (0, 0) to the actual cursor position.
    if (bridge->m_firstMouse)
    {
        bridge->m_lastMouseX = xpos;
        bridge->m_lastMouseY = ypos;
        bridge->m_firstMouse = false;
    }

    double dx = xpos - bridge->m_lastMouseX;
    double dy = ypos - bridge->m_lastMouseY;

    bridge->m_lastMouseX = xpos;
    bridge->m_lastMouseY = ypos;

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = EventType::MouseMove;
    event.mouseMove.x = static_cast<int32_t>(xpos);
    event.mouseMove.y = static_cast<int32_t>(ypos);
    event.mouseMove.deltaX = static_cast<int32_t>(dx);
    event.mouseMove.deltaY = static_cast<int32_t>(dy);

    bridge->m_queue->push(event);
}

void GLFWBridge::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = EventType::MouseScroll;
    event.mouseScroll.deltaX = static_cast<float>(xoffset);
    event.mouseScroll.deltaY = static_cast<float>(yoffset);

    bridge->m_queue->push(event);
}

void GLFWBridge::windowSizeCallback(GLFWwindow *window, int width, int height)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = EventType::WindowResize;
    event.windowResize.width = static_cast<int32_t>(width);
    event.windowResize.height = static_cast<int32_t>(height);

    bridge->m_queue->push(event);
}

void GLFWBridge::windowFocusCallback(GLFWwindow *window, int focused)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = focused ? EventType::WindowFocusGained : EventType::WindowFocusLost;

    bridge->m_queue->push(event);
}

void GLFWBridge::windowCloseCallback(GLFWwindow *window)
{
    auto *bridge = static_cast<GLFWBridge *>(glfwGetWindowUserPointer(window));
    if (!bridge)
    {
        return;
    }

    Event event{};
    event.timestampMs = currentTimeMs();
    event.type = EventType::WindowClose;

    bridge->m_queue->push(event);
}

// =========================================================================
// Key mapping
// =========================================================================

KeyCode GLFWBridge::mapGLFWKey(int glfwKey)
{
    switch (glfwKey)
    {
    // Letters
    case GLFW_KEY_A:
        return KeyCode::A;
    case GLFW_KEY_B:
        return KeyCode::B;
    case GLFW_KEY_C:
        return KeyCode::C;
    case GLFW_KEY_D:
        return KeyCode::D;
    case GLFW_KEY_E:
        return KeyCode::E;
    case GLFW_KEY_F:
        return KeyCode::F;
    case GLFW_KEY_G:
        return KeyCode::G;
    case GLFW_KEY_H:
        return KeyCode::H;
    case GLFW_KEY_I:
        return KeyCode::I;
    case GLFW_KEY_J:
        return KeyCode::J;
    case GLFW_KEY_K:
        return KeyCode::K;
    case GLFW_KEY_L:
        return KeyCode::L;
    case GLFW_KEY_M:
        return KeyCode::M;
    case GLFW_KEY_N:
        return KeyCode::N;
    case GLFW_KEY_O:
        return KeyCode::O;
    case GLFW_KEY_P:
        return KeyCode::P;
    case GLFW_KEY_Q:
        return KeyCode::Q;
    case GLFW_KEY_R:
        return KeyCode::R;
    case GLFW_KEY_S:
        return KeyCode::S;
    case GLFW_KEY_T:
        return KeyCode::T;
    case GLFW_KEY_U:
        return KeyCode::U;
    case GLFW_KEY_V:
        return KeyCode::V;
    case GLFW_KEY_W:
        return KeyCode::W;
    case GLFW_KEY_X:
        return KeyCode::X;
    case GLFW_KEY_Y:
        return KeyCode::Y;
    case GLFW_KEY_Z:
        return KeyCode::Z;

    // Top-row digits
    case GLFW_KEY_0:
        return KeyCode::Num0;
    case GLFW_KEY_1:
        return KeyCode::Num1;
    case GLFW_KEY_2:
        return KeyCode::Num2;
    case GLFW_KEY_3:
        return KeyCode::Num3;
    case GLFW_KEY_4:
        return KeyCode::Num4;
    case GLFW_KEY_5:
        return KeyCode::Num5;
    case GLFW_KEY_6:
        return KeyCode::Num6;
    case GLFW_KEY_7:
        return KeyCode::Num7;
    case GLFW_KEY_8:
        return KeyCode::Num8;
    case GLFW_KEY_9:
        return KeyCode::Num9;

    // Function keys
    case GLFW_KEY_F1:
        return KeyCode::F1;
    case GLFW_KEY_F2:
        return KeyCode::F2;
    case GLFW_KEY_F3:
        return KeyCode::F3;
    case GLFW_KEY_F4:
        return KeyCode::F4;
    case GLFW_KEY_F5:
        return KeyCode::F5;
    case GLFW_KEY_F6:
        return KeyCode::F6;
    case GLFW_KEY_F7:
        return KeyCode::F7;
    case GLFW_KEY_F8:
        return KeyCode::F8;
    case GLFW_KEY_F9:
        return KeyCode::F9;
    case GLFW_KEY_F10:
        return KeyCode::F10;
    case GLFW_KEY_F11:
        return KeyCode::F11;
    case GLFW_KEY_F12:
        return KeyCode::F12;

    // Common keys
    case GLFW_KEY_SPACE:
        return KeyCode::Space;
    case GLFW_KEY_ENTER:
        return KeyCode::Enter;
    case GLFW_KEY_ESCAPE:
        return KeyCode::Escape;
    case GLFW_KEY_TAB:
        return KeyCode::Tab;
    case GLFW_KEY_BACKSPACE:
        return KeyCode::Backspace;
    case GLFW_KEY_DELETE:
        return KeyCode::Delete;
    case GLFW_KEY_INSERT:
        return KeyCode::Insert;

    // Navigation
    case GLFW_KEY_UP:
        return KeyCode::Up;
    case GLFW_KEY_DOWN:
        return KeyCode::Down;
    case GLFW_KEY_LEFT:
        return KeyCode::Left;
    case GLFW_KEY_RIGHT:
        return KeyCode::Right;
    case GLFW_KEY_HOME:
        return KeyCode::Home;
    case GLFW_KEY_END:
        return KeyCode::End;
    case GLFW_KEY_PAGE_UP:
        return KeyCode::PageUp;
    case GLFW_KEY_PAGE_DOWN:
        return KeyCode::PageDown;

    // Modifier keys
    case GLFW_KEY_LEFT_SHIFT:
        return KeyCode::LShift;
    case GLFW_KEY_RIGHT_SHIFT:
        return KeyCode::RShift;
    case GLFW_KEY_LEFT_CONTROL:
        return KeyCode::LCtrl;
    case GLFW_KEY_RIGHT_CONTROL:
        return KeyCode::RCtrl;
    case GLFW_KEY_LEFT_ALT:
        return KeyCode::LAlt;
    case GLFW_KEY_RIGHT_ALT:
        return KeyCode::RAlt;

    // Lock keys
    case GLFW_KEY_CAPS_LOCK:
        return KeyCode::CapsLock;
    case GLFW_KEY_NUM_LOCK:
        return KeyCode::NumLock;
    case GLFW_KEY_SCROLL_LOCK:
        return KeyCode::ScrollLock;

    // Punctuation / symbols
    case GLFW_KEY_MINUS:
        return KeyCode::Minus;
    case GLFW_KEY_EQUAL:
        return KeyCode::Equals;
    case GLFW_KEY_LEFT_BRACKET:
        return KeyCode::LeftBracket;
    case GLFW_KEY_RIGHT_BRACKET:
        return KeyCode::RightBracket;
    case GLFW_KEY_BACKSLASH:
        return KeyCode::Backslash;
    case GLFW_KEY_SEMICOLON:
        return KeyCode::Semicolon;
    case GLFW_KEY_APOSTROPHE:
        return KeyCode::Apostrophe;
    case GLFW_KEY_GRAVE_ACCENT:
        return KeyCode::Grave;
    case GLFW_KEY_COMMA:
        return KeyCode::Comma;
    case GLFW_KEY_PERIOD:
        return KeyCode::Period;
    case GLFW_KEY_SLASH:
        return KeyCode::Slash;

    // Keypad
    case GLFW_KEY_KP_0:
        return KeyCode::KP_0;
    case GLFW_KEY_KP_1:
        return KeyCode::KP_1;
    case GLFW_KEY_KP_2:
        return KeyCode::KP_2;
    case GLFW_KEY_KP_3:
        return KeyCode::KP_3;
    case GLFW_KEY_KP_4:
        return KeyCode::KP_4;
    case GLFW_KEY_KP_5:
        return KeyCode::KP_5;
    case GLFW_KEY_KP_6:
        return KeyCode::KP_6;
    case GLFW_KEY_KP_7:
        return KeyCode::KP_7;
    case GLFW_KEY_KP_8:
        return KeyCode::KP_8;
    case GLFW_KEY_KP_9:
        return KeyCode::KP_9;
    case GLFW_KEY_KP_DECIMAL:
        return KeyCode::KP_Decimal;
    case GLFW_KEY_KP_DIVIDE:
        return KeyCode::KP_Divide;
    case GLFW_KEY_KP_MULTIPLY:
        return KeyCode::KP_Multiply;
    case GLFW_KEY_KP_SUBTRACT:
        return KeyCode::KP_Subtract;
    case GLFW_KEY_KP_ADD:
        return KeyCode::KP_Add;
    case GLFW_KEY_KP_ENTER:
        return KeyCode::KP_Enter;

    default:
        return KeyCode::Unknown;
    }
}

// =========================================================================
// Mouse button mapping
// =========================================================================

MouseButton GLFWBridge::mapGLFWMouseButton(int glfwButton)
{
    switch (glfwButton)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        return MouseButton::Left;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        return MouseButton::Middle;
    case GLFW_MOUSE_BUTTON_RIGHT:
        return MouseButton::Right;
    case GLFW_MOUSE_BUTTON_4:
        return MouseButton::X1;
    case GLFW_MOUSE_BUTTON_5:
        return MouseButton::X2;
    default:
        return MouseButton::None;
    }
}

// =========================================================================
// Modifier mapping
// =========================================================================

uint16_t GLFWBridge::mapGLFWMods(int glfwMods)
{
    uint16_t result = 0;

    // GLFW does not distinguish left / right modifier keys in its
    // modifier bitmask, so we set both left and right bits (i.e. the
    // combined Shift / Ctrl / Alt flags).
    if (glfwMods & GLFW_MOD_SHIFT)
    {
        result |= static_cast<uint16_t>(Modifier::Shift);
    }
    if (glfwMods & GLFW_MOD_CONTROL)
    {
        result |= static_cast<uint16_t>(Modifier::Ctrl);
    }
    if (glfwMods & GLFW_MOD_ALT)
    {
        result |= static_cast<uint16_t>(Modifier::Alt);
    }
    if (glfwMods & GLFW_MOD_CAPS_LOCK)
    {
        result |= static_cast<uint16_t>(Modifier::Caps);
    }
    if (glfwMods & GLFW_MOD_NUM_LOCK)
    {
        result |= static_cast<uint16_t>(Modifier::Num);
    }

    return result;
}

// =========================================================================
// Time helper
// =========================================================================

uint64_t GLFWBridge::currentTimeMs()
{
    return static_cast<uint64_t>(glfwGetTime() * 1000.0);
}

} // namespace Atom::Input

#endif // ATOM_INPUT_HAS_GLFW
