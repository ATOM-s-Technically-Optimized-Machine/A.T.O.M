/**
 * @file TestInputState.cpp
 * @brief Unit tests for Atom::Input::InputState.
 */

#include "TestHelpers.hpp"
#include "inputs/InputState.hpp"

#include <cstring>

using namespace Atom::Input;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Build a KeyDown event for a specific key.
static Event makeKeyDown(KeyCode key, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type          = EventType::KeyDown;
    e.timestampMs   = ts;
    e.key.key       = key;
    e.key.isRepeat  = false;
    e.key.modifiers = 0;
    return e;
}

/// Build a KeyUp event for a specific key.
static Event makeKeyUp(KeyCode key, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type          = EventType::KeyUp;
    e.timestampMs   = ts;
    e.key.key       = key;
    e.key.isRepeat  = false;
    e.key.modifiers = 0;
    return e;
}

/// Build a MouseMove event.
static Event makeMouseMove(int32_t x, int32_t y,
                           int32_t dx, int32_t dy,
                           uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type              = EventType::MouseMove;
    e.timestampMs       = ts;
    e.mouseMove.x       = x;
    e.mouseMove.y       = y;
    e.mouseMove.deltaX  = dx;
    e.mouseMove.deltaY  = dy;
    return e;
}

/// Build a MouseButtonDown event.
static Event makeMouseButtonDown(MouseButton btn, int32_t x = 0,
                                 int32_t y = 0, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type                 = EventType::MouseButtonDown;
    e.timestampMs          = ts;
    e.mouseButton.button   = btn;
    e.mouseButton.x        = x;
    e.mouseButton.y        = y;
    e.mouseButton.clicks   = 1;
    return e;
}

/// Build a MouseScroll event.
static Event makeMouseScroll(float dx, float dy, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type               = EventType::MouseScroll;
    e.timestampMs        = ts;
    e.mouseScroll.deltaX = dx;
    e.mouseScroll.deltaY = dy;
    return e;
}

/// Build a WindowFocusLost event.
static Event makeWindowFocusLost(uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type        = EventType::WindowFocusLost;
    e.timestampMs = ts;
    return e;
}

// =========================================================================
// Tests
// =========================================================================

static int test_key_press()
{
    InputState state;
    state.beginFrame(0);

    state.processEvent(makeKeyDown(KeyCode::A));

    ASSERT_TRUE(state.isKeyDown(KeyCode::A));
    ASSERT_TRUE(state.isKeyJustPressed(KeyCode::A));
    ASSERT_FALSE(state.isKeyJustReleased(KeyCode::A));

    return 0;
}

static int test_key_release()
{
    InputState state;

    // Frame 1: press A.
    state.beginFrame(0);
    state.processEvent(makeKeyDown(KeyCode::A));
    state.endFrame();

    // Frame 2: release A.
    state.beginFrame(16);
    state.processEvent(makeKeyUp(KeyCode::A));

    ASSERT_TRUE(state.isKeyJustReleased(KeyCode::A));
    ASSERT_FALSE(state.isKeyDown(KeyCode::A));

    return 0;
}

static int test_key_held()
{
    InputState state;

    // Frame 1: press A.
    state.beginFrame(0);
    state.processEvent(makeKeyDown(KeyCode::A));
    state.endFrame();

    // Frame 2: no key events -- A is still down.
    state.beginFrame(16);

    ASSERT_TRUE(state.isKeyDown(KeyCode::A));
    ASSERT_FALSE(state.isKeyJustPressed(KeyCode::A)); // Held, not Pressed.
    ASSERT_EQ(state.getKeyState(KeyCode::A), KeyState::Held);

    return 0;
}

static int test_mouse_move()
{
    InputState state;
    state.beginFrame(0);

    state.processEvent(makeMouseMove(100, 200, 10, 20));

    glm::vec2 pos   = state.mousePosition();
    glm::vec2 delta = state.mouseDelta();

    ASSERT_FLOAT_NEAR(pos.x,   100.0f, 0.01f);
    ASSERT_FLOAT_NEAR(pos.y,   200.0f, 0.01f);
    ASSERT_FLOAT_NEAR(delta.x,  10.0f, 0.01f);
    ASSERT_FLOAT_NEAR(delta.y,  20.0f, 0.01f);

    return 0;
}

static int test_mouse_button()
{
    InputState state;
    state.beginFrame(0);

    state.processEvent(makeMouseButtonDown(MouseButton::Left));

    ASSERT_TRUE(state.isMouseButtonDown(MouseButton::Left));
    ASSERT_FALSE(state.isMouseButtonDown(MouseButton::Right));

    return 0;
}

static int test_mouse_scroll()
{
    InputState state;
    state.beginFrame(0);

    state.processEvent(makeMouseScroll(0.0f, 3.0f));
    state.endFrame();

    glm::vec2 scroll = state.mouseScroll();

    ASSERT_FLOAT_NEAR(scroll.x, 0.0f, 0.01f);
    ASSERT_FLOAT_NEAR(scroll.y, 3.0f, 0.01f);

    return 0;
}

static int test_modifiers()
{
    InputState state;
    state.beginFrame(0);

    state.processEvent(makeKeyDown(KeyCode::LShift));

    ASSERT_TRUE(state.isShiftDown());
    ASSERT_TRUE(state.isModifierActive(Modifier::LShift));
    ASSERT_FALSE(state.isCtrlDown());
    ASSERT_FALSE(state.isAltDown());

    return 0;
}

static int test_delta_time()
{
    InputState state;

    state.beginFrame(0);
    state.endFrame();

    state.beginFrame(16);

    ASSERT_FLOAT_NEAR(state.deltaTime(), 0.016f, 0.001f);

    return 0;
}

static int test_window_focus()
{
    InputState state;

    ASSERT_TRUE(state.isWindowFocused());

    state.beginFrame(0);
    state.processEvent(makeWindowFocusLost());

    ASSERT_FALSE(state.isWindowFocused());

    return 0;
}

static int test_movement_vector()
{
    InputState state;
    state.beginFrame(0);

    // Press W (+Z) and D (+X).
    state.processEvent(makeKeyDown(KeyCode::W));
    state.processEvent(makeKeyDown(KeyCode::D));

    glm::vec3 mv = state.getMovementVector();

    // Should be normalised.
    ASSERT_FLOAT_NEAR(glm::length(mv), 1.0f, 0.01f);

    // W = +Z, D = +X.
    ASSERT_TRUE(mv.z > 0.0f);
    ASSERT_TRUE(mv.x > 0.0f);
    ASSERT_FLOAT_NEAR(mv.y, 0.0f, 0.001f);

    // Both components should be roughly equal (diagonal).
    ASSERT_FLOAT_NEAR(mv.x, mv.z, 0.01f);

    return 0;
}

// =========================================================================
// main
// =========================================================================

int main()
{
    int failures = 0;

    std::printf("TestInputState\n");

    RUN_TEST(test_key_press);
    RUN_TEST(test_key_release);
    RUN_TEST(test_key_held);
    RUN_TEST(test_mouse_move);
    RUN_TEST(test_mouse_button);
    RUN_TEST(test_mouse_scroll);
    RUN_TEST(test_modifiers);
    RUN_TEST(test_delta_time);
    RUN_TEST(test_window_focus);
    RUN_TEST(test_movement_vector);

    std::printf("\n  %d failure(s)\n", failures);
    return failures;
}
