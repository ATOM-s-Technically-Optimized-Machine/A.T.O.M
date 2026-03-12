/**
 * @file InputState.cpp
 * @brief Implementation of the Atom::Input::InputState class.
 */

#include "inputs/InputState.hpp"

#include <algorithm>
#include <cstring>
#include <glm/geometric.hpp>

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// Lifetime
// ---------------------------------------------------------------------------

InputState::InputState()
{
    reset();
}

// ---------------------------------------------------------------------------
// Frame lifecycle
// ---------------------------------------------------------------------------

void InputState::beginFrame(uint64_t frameTimeMs)
{
    // Snapshot previous key / button state.
    m_keysPrev = m_keys;
    m_buttonsPrev = m_buttons;

    // Compute delta time in seconds, clamped to avoid huge spikes
    // (e.g. after a debugger pause).
    m_prevTimeMs = m_currentTimeMs;
    m_currentTimeMs = frameTimeMs;

    float dt = static_cast<float>(frameTimeMs - m_prevTimeMs) / 1000.0f;
    m_deltaTime = std::clamp(dt, 0.0f, 0.1f);

    // Reset per-frame accumulators.
    m_mouseDelta = glm::vec2{0.0f};
    m_scrollAccum = glm::vec2{0.0f};
}

void InputState::endFrame()
{
    // Expose the accumulated scroll to queries until the next frame.
    m_scroll = m_scrollAccum;
}

// ---------------------------------------------------------------------------
// Event dispatch
// ---------------------------------------------------------------------------

void InputState::processEvent(const Event &event)
{
    switch (event.type)
    {
    case EventType::KeyDown:
    {
        auto idx = static_cast<std::size_t>(event.key.key);
        if (idx < kKeyCount)
        {
            // Record initial press time only on fresh press.
            if (!m_keys[idx])
            {
                m_keyPressTime[idx] = m_currentTimeMs;
            }
            m_keys[idx] = true;
            updateModifiers(event.key.key, true);
        }
        break;
    }

    case EventType::KeyUp:
    {
        auto idx = static_cast<std::size_t>(event.key.key);
        if (idx < kKeyCount)
        {
            m_keys[idx] = false;
            updateModifiers(event.key.key, false);
        }
        break;
    }

    case EventType::KeyRepeat:
    {
        // Key-repeat events are only relevant for text input;
        // input-state tracking intentionally ignores them.
        break;
    }

    case EventType::MouseButtonDown:
    {
        auto idx = static_cast<std::size_t>(event.mouseButton.button);
        if (idx < kButtonCount)
        {
            m_buttons[idx] = true;
        }
        m_mousePos.x = static_cast<float>(event.mouseButton.x);
        m_mousePos.y = static_cast<float>(event.mouseButton.y);
        break;
    }

    case EventType::MouseButtonUp:
    {
        auto idx = static_cast<std::size_t>(event.mouseButton.button);
        if (idx < kButtonCount)
        {
            m_buttons[idx] = false;
        }
        m_mousePos.x = static_cast<float>(event.mouseButton.x);
        m_mousePos.y = static_cast<float>(event.mouseButton.y);
        break;
    }

    case EventType::MouseMove:
    {
        m_mouseDelta.x += static_cast<float>(event.mouseMove.deltaX);
        m_mouseDelta.y += static_cast<float>(event.mouseMove.deltaY);
        m_mousePos.x = static_cast<float>(event.mouseMove.x);
        m_mousePos.y = static_cast<float>(event.mouseMove.y);
        break;
    }

    case EventType::MouseScroll:
    {
        m_scrollAccum.x += event.mouseScroll.deltaX;
        m_scrollAccum.y += event.mouseScroll.deltaY;
        break;
    }

    case EventType::WindowFocusGained:
    {
        m_windowFocused = true;
        break;
    }

    case EventType::WindowFocusLost:
    {
        m_windowFocused = false;

        // Release every key and button so nothing stays "stuck".
        m_keys.fill(false);
        m_buttons.fill(false);
        m_modifiers = 0;
        break;
    }

    case EventType::WindowResize:
    {
        m_windowWidth = event.windowResize.width;
        m_windowHeight = event.windowResize.height;
        break;
    }

    case EventType::WindowClose:
    {
        // Handled at a higher level (InputSystem); nothing to do.
        break;
    }

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------

void InputState::reset()
{
    m_keys.fill(false);
    m_keysPrev.fill(false);
    m_keyPressTime.fill(0);

    m_mousePos = glm::vec2{0.0f};
    m_mousePrevPos = glm::vec2{0.0f};
    m_mouseDelta = glm::vec2{0.0f};
    m_scroll = glm::vec2{0.0f};
    m_scrollAccum = glm::vec2{0.0f};

    m_buttons.fill(false);
    m_buttonsPrev.fill(false);

    m_modifiers = 0;
    m_deltaTime = 0.0f;
    m_currentTimeMs = 0;
    m_prevTimeMs = 0;
    m_windowFocused = true;
    m_windowWidth = 0;
    m_windowHeight = 0;
}

// ---------------------------------------------------------------------------
// Keyboard queries
// ---------------------------------------------------------------------------

bool InputState::isKeyDown(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    return idx < kKeyCount && m_keys[idx];
}

bool InputState::isKeyUp(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    return idx >= kKeyCount || !m_keys[idx];
}

bool InputState::isKeyJustPressed(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    return idx < kKeyCount && m_keys[idx] && !m_keysPrev[idx];
}

bool InputState::isKeyJustReleased(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    return idx < kKeyCount && !m_keys[idx] && m_keysPrev[idx];
}

KeyState InputState::getKeyState(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    if (idx >= kKeyCount)
    {
        return KeyState::Up;
    }

    bool curr = m_keys[idx];
    bool prev = m_keysPrev[idx];

    if (curr && !prev)
    {
        return KeyState::Pressed;
    }
    if (curr && prev)
    {
        return KeyState::Held;
    }
    if (!curr && prev)
    {
        return KeyState::Released;
    }
    return KeyState::Up;
}

float InputState::keyHeldDuration(KeyCode key) const noexcept
{
    auto idx = static_cast<std::size_t>(key);
    if (idx >= kKeyCount || !m_keys[idx])
    {
        return 0.0f;
    }
    return static_cast<float>(m_currentTimeMs - m_keyPressTime[idx]) / 1000.0f;
}

bool InputState::isAnyKeyDown() const noexcept
{
    for (std::size_t i = 0; i < kKeyCount; ++i)
    {
        if (m_keys[i])
        {
            return true;
        }
    }
    return false;
}

bool InputState::isAnyKeyJustPressed() const noexcept
{
    for (std::size_t i = 0; i < kKeyCount; ++i)
    {
        if (m_keys[i] && !m_keysPrev[i])
        {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Mouse queries
// ---------------------------------------------------------------------------

glm::vec2 InputState::mousePosition() const noexcept
{
    return m_mousePos;
}

glm::vec2 InputState::mouseDelta() const noexcept
{
    return m_mouseDelta;
}

glm::vec2 InputState::mouseScroll() const noexcept
{
    return m_scroll;
}

bool InputState::isMouseButtonDown(MouseButton btn) const noexcept
{
    auto idx = static_cast<std::size_t>(btn);
    return idx < kButtonCount && m_buttons[idx];
}

bool InputState::isMouseButtonUp(MouseButton btn) const noexcept
{
    auto idx = static_cast<std::size_t>(btn);
    return idx >= kButtonCount || !m_buttons[idx];
}

bool InputState::isMouseButtonJustPressed(MouseButton btn) const noexcept
{
    auto idx = static_cast<std::size_t>(btn);
    return idx < kButtonCount && m_buttons[idx] && !m_buttonsPrev[idx];
}

bool InputState::isMouseButtonJustReleased(MouseButton btn) const noexcept
{
    auto idx = static_cast<std::size_t>(btn);
    return idx < kButtonCount && !m_buttons[idx] && m_buttonsPrev[idx];
}

bool InputState::isAnyMouseButtonDown() const noexcept
{
    for (std::size_t i = 0; i < kButtonCount; ++i)
    {
        if (m_buttons[i])
        {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Modifier queries
// ---------------------------------------------------------------------------

bool InputState::isModifierActive(Modifier mod) const noexcept
{
    auto bits = static_cast<uint16_t>(mod);
    return (m_modifiers & bits) == bits;
}

bool InputState::isShiftDown() const noexcept
{
    return (m_modifiers & static_cast<uint16_t>(Modifier::Shift)) != 0;
}

bool InputState::isCtrlDown() const noexcept
{
    return (m_modifiers & static_cast<uint16_t>(Modifier::Ctrl)) != 0;
}

bool InputState::isAltDown() const noexcept
{
    return (m_modifiers & static_cast<uint16_t>(Modifier::Alt)) != 0;
}

// ---------------------------------------------------------------------------
// Movement helpers
// ---------------------------------------------------------------------------

glm::vec3 InputState::getMovementVector() const noexcept
{
    float x = 0.0f;
    float z = 0.0f;

    if (isKeyDown(KeyCode::W))
    {
        z += 1.0f;
    }
    if (isKeyDown(KeyCode::S))
    {
        z -= 1.0f;
    }
    if (isKeyDown(KeyCode::A))
    {
        x -= 1.0f;
    }
    if (isKeyDown(KeyCode::D))
    {
        x += 1.0f;
    }

    glm::vec3 dir{x, 0.0f, z};
    float len = glm::length(dir);
    if (len > 0.0f)
    {
        dir = glm::normalize(dir);
    }
    return dir;
}

float InputState::getVerticalMovement() const noexcept
{
    float y = 0.0f;
    if (isKeyDown(KeyCode::Space))
    {
        y += 1.0f;
    }
    if (isKeyDown(KeyCode::LCtrl))
    {
        y -= 1.0f;
    }
    return y;
}

glm::vec3 InputState::getFullMovementVector() const noexcept
{
    glm::vec3 horiz = getMovementVector();
    float vert = getVerticalMovement();

    glm::vec3 full{horiz.x, vert, horiz.z};
    float len = glm::length(full);
    if (len > 0.0f)
    {
        full = glm::normalize(full);
    }
    return full;
}

// ---------------------------------------------------------------------------
// Timing
// ---------------------------------------------------------------------------

float InputState::deltaTime() const noexcept
{
    return m_deltaTime;
}

uint64_t InputState::currentTimeMs() const noexcept
{
    return m_currentTimeMs;
}

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------

bool InputState::isWindowFocused() const noexcept
{
    return m_windowFocused;
}

glm::ivec2 InputState::windowSize() const noexcept
{
    return glm::ivec2{m_windowWidth, m_windowHeight};
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void InputState::updateModifiers(KeyCode key, bool pressed)
{
    Modifier flag = Modifier::None;

    switch (key)
    {
    case KeyCode::LShift:
        flag = Modifier::LShift;
        break;
    case KeyCode::RShift:
        flag = Modifier::RShift;
        break;
    case KeyCode::LCtrl:
        flag = Modifier::LCtrl;
        break;
    case KeyCode::RCtrl:
        flag = Modifier::RCtrl;
        break;
    case KeyCode::LAlt:
        flag = Modifier::LAlt;
        break;
    case KeyCode::RAlt:
        flag = Modifier::RAlt;
        break;
    default:
        return;
    }

    auto bits = static_cast<uint16_t>(flag);
    if (pressed)
    {
        m_modifiers |= bits;
    }
    else
    {
        m_modifiers &= static_cast<uint16_t>(~bits);
    }
}

} // namespace Atom::Input
