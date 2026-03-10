/**
 * @file input_state.h
 * @brief Input state manager for ATOM game engine
 *
 * Tracks current state of all input devices (keyboard, mouse).
 * Provides edge detection (just pressed, just released) and
 * normalized movement vectors.
 */

#ifndef ATOM_INPUT_STATE_H_
#define ATOM_INPUT_STATE_H_

#include "atom_math.h"
#include "input_event.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * Key State
     * ============================================================================ */

    typedef enum atom_key_state
    {
        ATOM_KEY_STATE_UP = 0,  /* Key is not pressed */
        ATOM_KEY_STATE_PRESSED, /* Key was just pressed this frame */
        ATOM_KEY_STATE_HELD,    /* Key is being held down */
        ATOM_KEY_STATE_RELEASED /* Key was just released this frame */
    } atom_key_state_t;

    /* ============================================================================
     * Mouse State
     * ============================================================================ */

    typedef struct atom_mouse_state
    {
        /* Absolute position */
        int32_t x;
        int32_t y;

        /* Previous position */
        int32_t prev_x;
        int32_t prev_y;

        /* Relative motion (delta) */
        int32_t delta_x;
        int32_t delta_y;

        /* Scroll wheel */
        float scroll_x;
        float scroll_y;

        /* Accumulated scroll (reset each frame) */
        float scroll_accum_x;
        float scroll_accum_y;

        /* Button states */
        bool buttons[ATOM_MOUSE_BUTTON_COUNT];
        bool buttons_prev[ATOM_MOUSE_BUTTON_COUNT];

        /* Capture state (relative mode) */
        bool is_captured;
    } atom_mouse_state_t;

    /* ============================================================================
     * Input State Manager
     * ============================================================================ */

    typedef struct atom_input_state
    {
        /* Keyboard state - double buffered for edge detection */
        bool keys[ATOM_KEY_COUNT];
        bool keys_prev[ATOM_KEY_COUNT];

        /* Timestamps for held duration calculation */
        uint64_t key_press_time[ATOM_KEY_COUNT];

        /* Mouse state */
        atom_mouse_state_t mouse;

        /* Current modifier state */
        uint16_t modifiers;

        /* Timing */
        uint64_t current_time_ms;
        uint64_t prev_time_ms;
        float delta_time; /* Seconds since last frame */

        /* Window state */
        bool window_focused;
        int32_t window_width;
        int32_t window_height;
    } atom_input_state_t;

    /* ============================================================================
     * Input State API
     * ============================================================================ */

    /* Initialization */
    void atom_input_state_init(atom_input_state_t *state);
    void atom_input_state_reset(atom_input_state_t *state);

    /* Frame management */
    void atom_input_state_begin_frame(atom_input_state_t *state, uint64_t frame_time_ms);
    void atom_input_state_end_frame(atom_input_state_t *state);

    /* Process single event into state */
    void atom_input_state_process_event(atom_input_state_t *state, const atom_input_event_t *event);

    /* ============================================================================
     * Keyboard Queries
     * ============================================================================ */

    /* Basic state */
    bool atom_key_is_down(const atom_input_state_t *state, atom_keycode_t key);
    bool atom_key_is_up(const atom_input_state_t *state, atom_keycode_t key);

    /* Edge detection */
    bool atom_key_just_pressed(const atom_input_state_t *state, atom_keycode_t key);
    bool atom_key_just_released(const atom_input_state_t *state, atom_keycode_t key);

    /* Combined state */
    atom_key_state_t atom_key_get_state(const atom_input_state_t *state, atom_keycode_t key);

    /* Duration */
    uint64_t atom_key_held_duration_ms(const atom_input_state_t *state, atom_keycode_t key);
    float atom_key_held_duration_sec(const atom_input_state_t *state, atom_keycode_t key);

    /* Check if any key is pressed */
    bool atom_any_key_down(const atom_input_state_t *state);
    bool atom_any_key_just_pressed(const atom_input_state_t *state);

    /* ============================================================================
     * Mouse Queries
     * ============================================================================ */

    /* Position */
    void atom_mouse_get_position(const atom_input_state_t *state, int32_t *x, int32_t *y);
    atom_vec2_t atom_mouse_get_position_vec(const atom_input_state_t *state);

    /* Delta (relative motion) */
    void atom_mouse_get_delta(const atom_input_state_t *state, int32_t *dx, int32_t *dy);
    atom_vec2_t atom_mouse_get_delta_vec(const atom_input_state_t *state);

    /* Scroll */
    void atom_mouse_get_scroll(const atom_input_state_t *state, float *sx, float *sy);
    atom_vec2_t atom_mouse_get_scroll_vec(const atom_input_state_t *state);

    /* Button states */
    bool atom_mouse_button_is_down(const atom_input_state_t *state, atom_mouse_button_t button);
    bool atom_mouse_button_is_up(const atom_input_state_t *state, atom_mouse_button_t button);
    bool atom_mouse_button_just_pressed(const atom_input_state_t *state,
                                        atom_mouse_button_t button);
    bool atom_mouse_button_just_released(const atom_input_state_t *state,
                                         atom_mouse_button_t button);

    /* Check any button */
    bool atom_any_mouse_button_down(const atom_input_state_t *state);
    bool atom_any_mouse_button_just_pressed(const atom_input_state_t *state);

    /* ============================================================================
     * Modifier Queries
     * ============================================================================ */

    bool atom_modifier_is_active(const atom_input_state_t *state, uint16_t modifier);
    bool atom_shift_down(const atom_input_state_t *state);
    bool atom_ctrl_down(const atom_input_state_t *state);
    bool atom_alt_down(const atom_input_state_t *state);

    /* ============================================================================
     * Movement Helpers
     * ============================================================================ */

    /**
     * Get WASD movement vector (normalized to prevent diagonal speed boost)
     * Returns a 3D vector: X = left/right, Y = 0, Z = forward/backward
     *
     * Keys: W = -Z (forward), S = +Z (backward), A = -X (left), D = +X (right)
     */
    atom_vec3_t atom_input_get_movement_vector(const atom_input_state_t *state);

    /**
     * Get 2D movement vector for vertical movement
     * Returns: Y = Space/Ctrl vertical movement
     *
     * Keys: Space = +1 (up), Ctrl = -1 (down)
     */
    float atom_input_get_vertical_movement(const atom_input_state_t *state);

    /**
     * Get full 3D movement including vertical (WASD + Space/Ctrl)
     * Combines horizontal and vertical movement, normalized.
     */
    atom_vec3_t atom_input_get_full_movement_vector(const atom_input_state_t *state);

    /* ============================================================================
     * Timing
     * ============================================================================ */

    float atom_input_state_get_delta_time(const atom_input_state_t *state);
    uint64_t atom_input_state_get_time_ms(const atom_input_state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_INPUT_STATE_H_ */
