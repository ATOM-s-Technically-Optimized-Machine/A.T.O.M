/**
 * @file Event.cpp
 * @brief Implementation of EventQueue, EventDispatcher, and name look-up
 *        helpers for the ATOM input system.
 */

#include "inputs/Event.hpp"

#include <algorithm>
#include <cstring>

namespace Atom::Input
{

// =========================================================================
// keyCodeName
// =========================================================================

const char *keyCodeName(KeyCode key) noexcept
{
    static const char *const names[] = {/* 0  */ "Unknown",
                                        /* 1  */ "A",
                                        /* 2  */ "B",
                                        /* 3  */ "C",
                                        /* 4  */ "D",
                                        /* 5  */ "E",
                                        /* 6  */ "F",
                                        /* 7  */ "G",
                                        /* 8  */ "H",
                                        /* 9  */ "I",
                                        /* 10 */ "J",
                                        /* 11 */ "K",
                                        /* 12 */ "L",
                                        /* 13 */ "M",
                                        /* 14 */ "N",
                                        /* 15 */ "O",
                                        /* 16 */ "P",
                                        /* 17 */ "Q",
                                        /* 18 */ "R",
                                        /* 19 */ "S",
                                        /* 20 */ "T",
                                        /* 21 */ "U",
                                        /* 22 */ "V",
                                        /* 23 */ "W",
                                        /* 24 */ "X",
                                        /* 25 */ "Y",
                                        /* 26 */ "Z",
                                        /* 27 */ "Num0",
                                        /* 28 */ "Num1",
                                        /* 29 */ "Num2",
                                        /* 30 */ "Num3",
                                        /* 31 */ "Num4",
                                        /* 32 */ "Num5",
                                        /* 33 */ "Num6",
                                        /* 34 */ "Num7",
                                        /* 35 */ "Num8",
                                        /* 36 */ "Num9",
                                        /* 37 */ "F1",
                                        /* 38 */ "F2",
                                        /* 39 */ "F3",
                                        /* 40 */ "F4",
                                        /* 41 */ "F5",
                                        /* 42 */ "F6",
                                        /* 43 */ "F7",
                                        /* 44 */ "F8",
                                        /* 45 */ "F9",
                                        /* 46 */ "F10",
                                        /* 47 */ "F11",
                                        /* 48 */ "F12",
                                        /* 49 */ "Space",
                                        /* 50 */ "Enter",
                                        /* 51 */ "Escape",
                                        /* 52 */ "Tab",
                                        /* 53 */ "Backspace",
                                        /* 54 */ "Delete",
                                        /* 55 */ "Insert",
                                        /* 56 */ "Up",
                                        /* 57 */ "Down",
                                        /* 58 */ "Left",
                                        /* 59 */ "Right",
                                        /* 60 */ "Home",
                                        /* 61 */ "End",
                                        /* 62 */ "PageUp",
                                        /* 63 */ "PageDown",
                                        /* 64 */ "LShift",
                                        /* 65 */ "RShift",
                                        /* 66 */ "LCtrl",
                                        /* 67 */ "RCtrl",
                                        /* 68 */ "LAlt",
                                        /* 69 */ "RAlt",
                                        /* 70 */ "CapsLock",
                                        /* 71 */ "NumLock",
                                        /* 72 */ "ScrollLock",
                                        /* 73 */ "Minus",
                                        /* 74 */ "Equals",
                                        /* 75 */ "LeftBracket",
                                        /* 76 */ "RightBracket",
                                        /* 77 */ "Backslash",
                                        /* 78 */ "Semicolon",
                                        /* 79 */ "Apostrophe",
                                        /* 80 */ "Grave",
                                        /* 81 */ "Comma",
                                        /* 82 */ "Period",
                                        /* 83 */ "Slash",
                                        /* 84 */ "KP_0",
                                        /* 85 */ "KP_1",
                                        /* 86 */ "KP_2",
                                        /* 87 */ "KP_3",
                                        /* 88 */ "KP_4",
                                        /* 89 */ "KP_5",
                                        /* 90 */ "KP_6",
                                        /* 91 */ "KP_7",
                                        /* 92 */ "KP_8",
                                        /* 93 */ "KP_9",
                                        /* 94 */ "KP_Decimal",
                                        /* 95 */ "KP_Divide",
                                        /* 96 */ "KP_Multiply",
                                        /* 97 */ "KP_Subtract",
                                        /* 98 */ "KP_Add",
                                        /* 99 */ "KP_Enter"};

    auto idx = static_cast<uint16_t>(key);
    if (idx >= static_cast<uint16_t>(KeyCode::Count))
    {
        return "Unknown";
    }
    return names[idx];
}

// =========================================================================
// mouseButtonName
// =========================================================================

const char *mouseButtonName(MouseButton button) noexcept
{
    switch (button)
    {
    case MouseButton::Left:
        return "Left";
    case MouseButton::Middle:
        return "Middle";
    case MouseButton::Right:
        return "Right";
    case MouseButton::X1:
        return "X1";
    case MouseButton::X2:
        return "X2";
    default:
        return "None";
    }
}

// =========================================================================
// eventTypeName
// =========================================================================

const char *eventTypeName(EventType type) noexcept
{
    switch (type)
    {
    case EventType::KeyDown:
        return "KeyDown";
    case EventType::KeyUp:
        return "KeyUp";
    case EventType::KeyRepeat:
        return "KeyRepeat";
    case EventType::MouseButtonDown:
        return "MouseButtonDown";
    case EventType::MouseButtonUp:
        return "MouseButtonUp";
    case EventType::MouseMove:
        return "MouseMove";
    case EventType::MouseScroll:
        return "MouseScroll";
    case EventType::WindowFocusGained:
        return "WindowFocusGained";
    case EventType::WindowFocusLost:
        return "WindowFocusLost";
    case EventType::WindowResize:
        return "WindowResize";
    case EventType::WindowClose:
        return "WindowClose";
    default:
        return "Unknown";
    }
}

// =========================================================================
// EventQueue
// =========================================================================

EventQueue::EventQueue(std::size_t capacity) : m_head(0), m_tail(0), m_count(0)
{
    // Round up to the next power of two.
    // If capacity is already a power of two it stays unchanged.
    if (capacity == 0)
    {
        capacity = 1;
    }
    --capacity;
    capacity |= capacity >> 1;
    capacity |= capacity >> 2;
    capacity |= capacity >> 4;
    capacity |= capacity >> 8;
    capacity |= capacity >> 16;
    capacity |= capacity >> 32;
    ++capacity;

    m_capacity = capacity;
    m_buffer.resize(m_capacity);
}

bool EventQueue::push(const Event &event) noexcept
{
    if (isFull())
    {
        return false;
    }
    m_buffer[m_head & (m_capacity - 1)] = event;
    ++m_head;
    ++m_count;
    return true;
}

bool EventQueue::pop(Event &event) noexcept
{
    if (isEmpty())
    {
        return false;
    }
    event = m_buffer[m_tail & (m_capacity - 1)];
    ++m_tail;
    --m_count;
    return true;
}

bool EventQueue::peek(Event &event) const noexcept
{
    if (isEmpty())
    {
        return false;
    }
    event = m_buffer[m_tail & (m_capacity - 1)];
    return true;
}

bool EventQueue::isEmpty() const noexcept
{
    return m_count == 0;
}

bool EventQueue::isFull() const noexcept
{
    return m_count == m_capacity;
}

std::size_t EventQueue::count() const noexcept
{
    return m_count;
}

void EventQueue::clear() noexcept
{
    m_head = 0;
    m_tail = 0;
    m_count = 0;
}

// =========================================================================
// EventDispatcher
// =========================================================================

EventDispatcher::ListenerID EventDispatcher::addListener(EventType type, EventCallback callback)
{
    ListenerID id = m_nextID++;
    m_listeners.push_back({id, type, std::move(callback)});
    return id;
}

void EventDispatcher::removeListener(ListenerID id)
{
    m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(),
                                     [id](const ListenerEntry &entry) { return entry.id == id; }),
                      m_listeners.end());
}

void EventDispatcher::dispatch(const Event &event)
{
    for (const auto &entry : m_listeners)
    {
        if (entry.type == event.type)
        {
            entry.callback(event);
        }
    }
}

void EventDispatcher::clear()
{
    m_listeners.clear();
}

} // namespace Atom::Input
