/**
 * @file TestEvent.cpp
 * @brief Unit tests for Atom::Input::EventQueue and EventDispatcher.
 */

#include "TestHelpers.hpp"
#include "inputs/Event.hpp"

#include <cstring>

using namespace Atom::Input;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Build a minimal Event with only the type and timestamp filled in.
static Event makeEvent(EventType type, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type        = type;
    e.timestampMs = ts;
    return e;
}

/// Build a KeyDown event for a specific key.
static Event makeKeyDown(KeyCode key, uint64_t ts = 0)
{
    Event e = makeEvent(EventType::KeyDown, ts);
    e.key.key      = key;
    e.key.isRepeat  = false;
    e.key.modifiers = 0;
    return e;
}

// =========================================================================
// EventQueue tests
// =========================================================================

static int test_event_queue_push_pop()
{
    EventQueue q(4);

    Event e0 = makeEvent(EventType::KeyDown,   10);
    Event e1 = makeEvent(EventType::KeyUp,     20);
    Event e2 = makeEvent(EventType::MouseMove, 30);

    ASSERT_TRUE(q.push(e0));
    ASSERT_TRUE(q.push(e1));
    ASSERT_TRUE(q.push(e2));
    ASSERT_EQ(q.count(), static_cast<std::size_t>(3));

    Event out{};

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.type, EventType::KeyDown);
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(10));

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.type, EventType::KeyUp);
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(20));

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.type, EventType::MouseMove);
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(30));

    ASSERT_EQ(q.count(), static_cast<std::size_t>(0));
    ASSERT_TRUE(q.isEmpty());

    return 0;
}

static int test_event_queue_full()
{
    EventQueue q(4);

    // Push exactly 4 events (capacity is rounded up to 4).
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyDown, 1)));
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyDown, 2)));
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyDown, 3)));
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyDown, 4)));

    ASSERT_TRUE(q.isFull());
    ASSERT_EQ(q.count(), static_cast<std::size_t>(4));

    // 5th push must fail.
    ASSERT_FALSE(q.push(makeEvent(EventType::KeyDown, 5)));

    return 0;
}

static int test_event_queue_empty()
{
    EventQueue q(4);

    ASSERT_TRUE(q.isEmpty());
    ASSERT_EQ(q.count(), static_cast<std::size_t>(0));

    Event out{};
    ASSERT_FALSE(q.pop(out));

    return 0;
}

static int test_event_queue_clear()
{
    EventQueue q(4);

    q.push(makeEvent(EventType::KeyDown));
    q.push(makeEvent(EventType::KeyUp));
    ASSERT_FALSE(q.isEmpty());

    q.clear();

    ASSERT_TRUE(q.isEmpty());
    ASSERT_EQ(q.count(), static_cast<std::size_t>(0));

    return 0;
}

static int test_event_queue_wrap_around()
{
    EventQueue q(4); // capacity = 4

    // Push 3 events.
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyDown,   1)));
    ASSERT_TRUE(q.push(makeEvent(EventType::KeyUp,     2)));
    ASSERT_TRUE(q.push(makeEvent(EventType::MouseMove, 3)));

    // Pop 2 events -- this advances the tail past the beginning.
    Event out{};
    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(1));
    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(2));

    // Queue now has 1 element. Push 3 more to wrap around the buffer.
    ASSERT_TRUE(q.push(makeEvent(EventType::MouseScroll,       4)));
    ASSERT_TRUE(q.push(makeEvent(EventType::WindowFocusGained, 5)));
    ASSERT_TRUE(q.push(makeEvent(EventType::WindowResize,      6)));

    ASSERT_EQ(q.count(), static_cast<std::size_t>(4));
    ASSERT_TRUE(q.isFull());

    // Verify FIFO order: 3, 4, 5, 6.
    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(3));

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(4));

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(5));

    ASSERT_TRUE(q.pop(out));
    ASSERT_EQ(out.timestampMs, static_cast<uint64_t>(6));

    ASSERT_TRUE(q.isEmpty());

    return 0;
}

// =========================================================================
// EventDispatcher tests
// =========================================================================

static int test_event_dispatcher_basic()
{
    EventDispatcher dispatcher;

    bool called = false;
    EventType receivedType = EventType::Count;

    dispatcher.addListener(EventType::KeyDown,
        [&](const Event& e)
        {
            called       = true;
            receivedType = e.type;
        });

    Event e = makeKeyDown(KeyCode::A, 100);
    dispatcher.dispatch(e);

    ASSERT_TRUE(called);
    ASSERT_EQ(receivedType, EventType::KeyDown);

    return 0;
}

static int test_event_dispatcher_remove()
{
    EventDispatcher dispatcher;

    bool called = false;

    auto id = dispatcher.addListener(EventType::KeyDown,
        [&](const Event& /*e*/)
        {
            called = true;
        });

    dispatcher.removeListener(id);
    dispatcher.dispatch(makeKeyDown(KeyCode::A));

    ASSERT_FALSE(called);

    return 0;
}

static int test_event_dispatcher_multiple()
{
    EventDispatcher dispatcher;

    int callCount = 0;

    dispatcher.addListener(EventType::KeyDown,
        [&](const Event& /*e*/)
        {
            callCount++;
        });

    dispatcher.addListener(EventType::KeyDown,
        [&](const Event& /*e*/)
        {
            callCount++;
        });

    dispatcher.dispatch(makeKeyDown(KeyCode::A));

    ASSERT_EQ(callCount, 2);

    return 0;
}

static int test_event_type_name()
{
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::KeyDown),   "KeyDown"),   0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::KeyUp),     "KeyUp"),     0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::KeyRepeat), "KeyRepeat"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::MouseButtonDown),
                          "MouseButtonDown"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::MouseButtonUp),
                          "MouseButtonUp"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::MouseMove),
                          "MouseMove"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::MouseScroll),
                          "MouseScroll"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::WindowFocusGained),
                          "WindowFocusGained"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::WindowFocusLost),
                          "WindowFocusLost"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::WindowResize),
                          "WindowResize"), 0);
    ASSERT_EQ(std::strcmp(eventTypeName(EventType::WindowClose),
                          "WindowClose"), 0);

    return 0;
}

// =========================================================================
// main
// =========================================================================

int main()
{
    int failures = 0;

    std::printf("TestEvent\n");

    RUN_TEST(test_event_queue_push_pop);
    RUN_TEST(test_event_queue_full);
    RUN_TEST(test_event_queue_empty);
    RUN_TEST(test_event_queue_clear);
    RUN_TEST(test_event_queue_wrap_around);
    RUN_TEST(test_event_dispatcher_basic);
    RUN_TEST(test_event_dispatcher_remove);
    RUN_TEST(test_event_dispatcher_multiple);
    RUN_TEST(test_event_type_name);

    std::printf("\n  %d failure(s)\n", failures);
    return failures;
}
