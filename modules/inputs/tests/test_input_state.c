/**
 * @file test_input_state.c
 * @brief Unit tests for input_state: keyboard/mouse state, edge detection, movement
 */

#include "test_helpers.h"
#include <inputs/input_state.h>

/* Helper: create a key down event */
static atom_input_event_t make_key_event(atom_keycode_t key, atom_input_event_type_t type,
                                          uint64_t time)
{
    atom_input_event_t event = {0};
    event.type = type;
    event.timestamp_ms = time;
    event.data.key.key = key;
    return event;
}

/* Helper: create a mouse button event */
static atom_input_event_t make_mouse_button_event(atom_mouse_button_t button,
                                                    atom_input_event_type_t type,
                                                    int32_t x, int32_t y)
{
    atom_input_event_t event = {0};
    event.type = type;
    event.timestamp_ms = 0;
    event.data.mouse_button.button = button;
    event.data.mouse_button.x = x;
    event.data.mouse_button.y = y;
    return event;
}

/* Helper: create a mouse move event */
static atom_input_event_t make_mouse_move_event(int32_t x, int32_t y,
                                                  int32_t dx, int32_t dy)
{
    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_MOUSE_MOVE;
    event.data.mouse_move.x = x;
    event.data.mouse_move.y = y;
    event.data.mouse_move.delta_x = dx;
    event.data.mouse_move.delta_y = dy;
    return event;
}

/* ========================================================================== *
 * Initialization Tests
 * ========================================================================== */

static void test_state_init(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    /* All keys should be up */
    for (int i = 0; i < ATOM_KEY_COUNT; i++)
    {
        ASSERT_FALSE(atom_key_is_down(&state, (atom_keycode_t)i));
    }

    /* No mouse buttons */
    ASSERT_FALSE(atom_mouse_button_is_down(&state, ATOM_MOUSE_LEFT));
    ASSERT_FALSE(atom_mouse_button_is_down(&state, ATOM_MOUSE_RIGHT));

    /* No modifiers */
    ASSERT_FALSE(atom_shift_down(&state));
    ASSERT_FALSE(atom_ctrl_down(&state));
    ASSERT_FALSE(atom_alt_down(&state));
}

/* ========================================================================== *
 * Keyboard State Tests
 * ========================================================================== */

static void test_key_down_up(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press A */
    atom_input_event_t down = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &down);

    ASSERT_TRUE(atom_key_is_down(&state, ATOM_KEY_A));
    ASSERT_FALSE(atom_key_is_up(&state, ATOM_KEY_A));

    /* Release A (next frame) */
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 200);

    atom_input_event_t up = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_UP, 200);
    atom_input_state_process_event(&state, &up);

    ASSERT_FALSE(atom_key_is_down(&state, ATOM_KEY_A));
    ASSERT_TRUE(atom_key_is_up(&state, ATOM_KEY_A));
}

static void test_key_just_pressed(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    /* Frame 1: press A */
    atom_input_state_begin_frame(&state, 100);
    atom_input_event_t down = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &down);

    ASSERT_TRUE(atom_key_just_pressed(&state, ATOM_KEY_A));
    ASSERT_TRUE(atom_any_key_just_pressed(&state));

    /* Frame 2: A still held - not "just pressed" anymore */
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 200);

    ASSERT_TRUE(atom_key_is_down(&state, ATOM_KEY_A));
    ASSERT_FALSE(atom_key_just_pressed(&state, ATOM_KEY_A));
}

static void test_key_just_released(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    /* Frame 1: press A */
    atom_input_state_begin_frame(&state, 100);
    atom_input_event_t down = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &down);
    atom_input_state_end_frame(&state);

    /* Frame 2: release A */
    atom_input_state_begin_frame(&state, 200);
    atom_input_event_t up = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_UP, 200);
    atom_input_state_process_event(&state, &up);

    ASSERT_TRUE(atom_key_just_released(&state, ATOM_KEY_A));

    /* Frame 3: no longer "just released" */
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 300);
    ASSERT_FALSE(atom_key_just_released(&state, ATOM_KEY_A));
}

static void test_key_get_state(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    /* Initially UP */
    ASSERT_EQ(atom_key_get_state(&state, ATOM_KEY_W), ATOM_KEY_STATE_UP);

    /* Frame 1: press -> PRESSED */
    atom_input_state_begin_frame(&state, 100);
    atom_input_event_t down = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &down);
    ASSERT_EQ(atom_key_get_state(&state, ATOM_KEY_W), ATOM_KEY_STATE_PRESSED);

    /* Frame 2: still held -> HELD */
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 200);
    ASSERT_EQ(atom_key_get_state(&state, ATOM_KEY_W), ATOM_KEY_STATE_HELD);

    /* Frame 3: release -> RELEASED */
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 300);
    atom_input_event_t up = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_UP, 300);
    atom_input_state_process_event(&state, &up);
    ASSERT_EQ(atom_key_get_state(&state, ATOM_KEY_W), ATOM_KEY_STATE_RELEASED);
}

static void test_key_held_duration(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    /* Frame 1: press at t=1000 */
    atom_input_state_begin_frame(&state, 1000);
    atom_input_event_t down = make_key_event(ATOM_KEY_A, ATOM_EVENT_KEY_DOWN, 1000);
    atom_input_state_process_event(&state, &down);
    atom_input_state_end_frame(&state);

    /* Frame 2: check at t=1500 */
    atom_input_state_begin_frame(&state, 1500);
    uint64_t dur_ms = atom_key_held_duration_ms(&state, ATOM_KEY_A);
    ASSERT_EQ(dur_ms, 500u);

    float dur_sec = atom_key_held_duration_sec(&state, ATOM_KEY_A);
    ASSERT_FLOAT_EQ(dur_sec, 0.5f);
}

static void test_any_key_down(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    ASSERT_FALSE(atom_any_key_down(&state));

    atom_input_event_t down = make_key_event(ATOM_KEY_SPACE, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &down);

    ASSERT_TRUE(atom_any_key_down(&state));
}

/* ========================================================================== *
 * Mouse Tests
 * ========================================================================== */

static void test_mouse_button(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    atom_input_event_t down = make_mouse_button_event(ATOM_MOUSE_LEFT,
                                                       ATOM_EVENT_MOUSE_BUTTON_DOWN, 100, 200);
    atom_input_state_process_event(&state, &down);

    ASSERT_TRUE(atom_mouse_button_is_down(&state, ATOM_MOUSE_LEFT));
    ASSERT_TRUE(atom_mouse_button_just_pressed(&state, ATOM_MOUSE_LEFT));
    ASSERT_TRUE(atom_any_mouse_button_down(&state));
    ASSERT_FALSE(atom_mouse_button_is_down(&state, ATOM_MOUSE_RIGHT));
}

static void test_mouse_move(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    atom_input_event_t move = make_mouse_move_event(400, 300, 10, -5);
    atom_input_state_process_event(&state, &move);

    int32_t x, y;
    atom_mouse_get_position(&state, &x, &y);
    ASSERT_EQ(x, 400);
    ASSERT_EQ(y, 300);

    int32_t dx, dy;
    atom_mouse_get_delta(&state, &dx, &dy);
    ASSERT_EQ(dx, 10);
    ASSERT_EQ(dy, -5);
}

static void test_mouse_scroll(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    atom_input_event_t scroll = {0};
    scroll.type = ATOM_EVENT_MOUSE_SCROLL;
    scroll.data.mouse_scroll.delta_x = 0.0f;
    scroll.data.mouse_scroll.delta_y = 3.0f;
    atom_input_state_process_event(&state, &scroll);

    /* Scroll accumulates; check the accumulated value directly */
    ASSERT_FLOAT_EQ(state.mouse.scroll_accum_y, 3.0f);
}

static void test_mouse_position_vec(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    atom_input_event_t move = make_mouse_move_event(640, 480, 0, 0);
    atom_input_state_process_event(&state, &move);

    atom_vec2_t pos = atom_mouse_get_position_vec(&state);
    ASSERT_FLOAT_EQ(pos.x, 640.0f);
    ASSERT_FLOAT_EQ(pos.y, 480.0f);
}

/* ========================================================================== *
 * Modifier Tests
 * ========================================================================== */

static void test_modifiers(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press left shift */
    atom_input_event_t shift_down = make_key_event(ATOM_KEY_LSHIFT, ATOM_EVENT_KEY_DOWN, 100);
    shift_down.data.key.modifiers = ATOM_MOD_LSHIFT;
    atom_input_state_process_event(&state, &shift_down);

    ASSERT_TRUE(atom_shift_down(&state));
    ASSERT_TRUE(atom_modifier_is_active(&state, ATOM_MOD_SHIFT));
    ASSERT_FALSE(atom_ctrl_down(&state));
    ASSERT_FALSE(atom_alt_down(&state));
}

/* ========================================================================== *
 * Movement Helper Tests
 * ========================================================================== */

static void test_movement_vector_forward(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press W (forward) */
    atom_input_event_t w_down = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &w_down);

    atom_vec3_t movement = atom_input_get_movement_vector(&state);
    /* W = forward = -Z */
    ASSERT_TRUE(movement.z < 0.0f);
    ASSERT_FLOAT_EQ(movement.y, 0.0f);
}

static void test_movement_vector_normalized_diagonal(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press W + D (forward-right diagonal) */
    atom_input_event_t w_down = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_event_t d_down = make_key_event(ATOM_KEY_D, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &w_down);
    atom_input_state_process_event(&state, &d_down);

    atom_vec3_t movement = atom_input_get_movement_vector(&state);
    /* Should be normalized: length <= 1 (no diagonal speed boost) */
    float len = atom_vec3_length(movement);
    ASSERT_TRUE(len <= 1.0f + TEST_EPSILON);
    ASSERT_TRUE(len > 0.9f);
}

static void test_movement_vector_cancel(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press W + S (forward + backward = cancel) */
    atom_input_event_t w_down = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_event_t s_down = make_key_event(ATOM_KEY_S, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &w_down);
    atom_input_state_process_event(&state, &s_down);

    atom_vec3_t movement = atom_input_get_movement_vector(&state);
    ASSERT_FLOAT_EQ(atom_vec3_length(movement), 0.0f);
}

static void test_vertical_movement(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press Space (up) */
    atom_input_event_t space = make_key_event(ATOM_KEY_SPACE, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &space);

    float vert = atom_input_get_vertical_movement(&state);
    ASSERT_TRUE(vert > 0.0f);
}

static void test_full_movement_vector(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Press W + Space */
    atom_input_event_t w = make_key_event(ATOM_KEY_W, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_event_t space = make_key_event(ATOM_KEY_SPACE, ATOM_EVENT_KEY_DOWN, 100);
    atom_input_state_process_event(&state, &w);
    atom_input_state_process_event(&state, &space);

    atom_vec3_t movement = atom_input_get_full_movement_vector(&state);
    /* Should have both forward (negative Z) and up (positive Y) */
    ASSERT_TRUE(movement.z < 0.0f);
    ASSERT_TRUE(movement.y > 0.0f);
    /* Should be normalized */
    ASSERT_TRUE(atom_vec3_length(movement) <= 1.0f + TEST_EPSILON);
}

/* ========================================================================== *
 * Timing Tests
 * ========================================================================== */

static void test_delta_time(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);

    atom_input_state_begin_frame(&state, 1000);
    atom_input_state_end_frame(&state);
    atom_input_state_begin_frame(&state, 1016);

    float dt = atom_input_state_get_delta_time(&state);
    ASSERT_FLOAT_NEAR(dt, 0.016f, 0.001f);
}

static void test_window_events(void)
{
    atom_input_state_t state;
    atom_input_state_init(&state);
    atom_input_state_begin_frame(&state, 100);

    /* Window resize */
    atom_input_event_t resize = {0};
    resize.type = ATOM_EVENT_WINDOW_RESIZE;
    resize.data.window_resize.width = 1920;
    resize.data.window_resize.height = 1080;
    atom_input_state_process_event(&state, &resize);

    ASSERT_EQ(state.window_width, 1920);
    ASSERT_EQ(state.window_height, 1080);

    /* Window focus */
    atom_input_event_t focus = {0};
    focus.type = ATOM_EVENT_WINDOW_FOCUS_GAINED;
    atom_input_state_process_event(&state, &focus);
    ASSERT_TRUE(state.window_focused);

    atom_input_event_t unfocus = {0};
    unfocus.type = ATOM_EVENT_WINDOW_FOCUS_LOST;
    atom_input_state_process_event(&state, &unfocus);
    ASSERT_FALSE(state.window_focused);
}

/* ========================================================================== *
 * Main
 * ========================================================================== */

int main(void)
{
    printf("=== input_state tests ===\n\n");

    printf("[Initialization]\n");
    RUN_TEST(test_state_init);

    printf("\n[Keyboard]\n");
    RUN_TEST(test_key_down_up);
    RUN_TEST(test_key_just_pressed);
    RUN_TEST(test_key_just_released);
    RUN_TEST(test_key_get_state);
    RUN_TEST(test_key_held_duration);
    RUN_TEST(test_any_key_down);

    printf("\n[Mouse]\n");
    RUN_TEST(test_mouse_button);
    RUN_TEST(test_mouse_move);
    RUN_TEST(test_mouse_scroll);
    RUN_TEST(test_mouse_position_vec);

    printf("\n[Modifiers]\n");
    RUN_TEST(test_modifiers);

    printf("\n[Movement]\n");
    RUN_TEST(test_movement_vector_forward);
    RUN_TEST(test_movement_vector_normalized_diagonal);
    RUN_TEST(test_movement_vector_cancel);
    RUN_TEST(test_vertical_movement);
    RUN_TEST(test_full_movement_vector);

    printf("\n[Timing & Window]\n");
    RUN_TEST(test_delta_time);
    RUN_TEST(test_window_events);

    TEST_SUMMARY();
}
