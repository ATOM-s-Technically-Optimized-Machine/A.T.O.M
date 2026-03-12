/**
 * @file InputSystem.cpp
 * @brief Implementation of the Atom::Input::InputSystem class.
 */

#include "inputs/InputSystem.hpp"

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// Lifetime
// ---------------------------------------------------------------------------

InputSystem::InputSystem() = default;

InputSystem::~InputSystem() = default;

// ---------------------------------------------------------------------------
// Frame lifecycle
// ---------------------------------------------------------------------------

void InputSystem::beginFrame(uint64_t frameTimeMs)
{
    m_state.beginFrame(frameTimeMs);
}

void InputSystem::processEvents()
{
    Event event{};
    while (m_queue.pop(event))
    {
        // Dispatch to user-registered listeners first.
        m_dispatcher.dispatch(event);

        // Feed the aggregated input state.
        m_state.processEvent(event);

        // Check for window-close request.
        if (event.type == EventType::WindowClose)
        {
            m_shouldQuit = true;
        }
    }
}

void InputSystem::endFrame()
{
    m_state.endFrame();
}

void InputSystem::update(uint64_t frameTimeMs)
{
    beginFrame(frameTimeMs);
    processEvents();
    endFrame();
}

// ---------------------------------------------------------------------------
// Queue access
// ---------------------------------------------------------------------------

EventQueue &InputSystem::eventQueue() noexcept
{
    return m_queue;
}

// ---------------------------------------------------------------------------
// Listener management
// ---------------------------------------------------------------------------

EventDispatcher::ListenerID InputSystem::addListener(EventType type,
                                                     EventDispatcher::EventCallback callback)
{
    return m_dispatcher.addListener(type, std::move(callback));
}

void InputSystem::removeListener(EventDispatcher::ListenerID id)
{
    m_dispatcher.removeListener(id);
}

// ---------------------------------------------------------------------------
// State queries
// ---------------------------------------------------------------------------

const InputState &InputSystem::state() const noexcept
{
    return m_state;
}

float InputSystem::deltaTime() const noexcept
{
    return m_state.deltaTime();
}

bool InputSystem::shouldQuit() const noexcept
{
    return m_shouldQuit;
}

// ---------------------------------------------------------------------------
// Mouse capture
// ---------------------------------------------------------------------------

void InputSystem::captureMouse(bool captured)
{
    m_mouseCaptured = captured;
}

bool InputSystem::isMouseCaptured() const noexcept
{
    return m_mouseCaptured;
}

} // namespace Atom::Input
