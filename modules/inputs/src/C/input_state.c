/**
 * @file input_state.c
 * @brief Implementation of input state manager
 */

#include "inputs/input_state.h"

#include <string.h>

/* ============================================================================
 * Initialization
 * ============================================================================ */

void atom_input_state_init(atom_input_state_t *state)
{
    if (!state)
        return;
    memset(state, 0, sizeof(atom_input_state_t));
    state->window_focused = true;
}

void atom_input_state_reset(atom_input_state_t *state)
{
    if (!state)
        return;

    memset(state->keys, 0, sizeof(state->keys));
    memset(state->keys_prev, 0, sizeof(state->keys_prev));
    memset(state->key_press_time, 0, sizeof(state->key_press_time));

    memset(&state->mouse, 0, sizeof(state->mouse));
    state->modifiers = ATOM_MOD_NONE;
}

/* ============================================================================
 * Frame Management
 * ============================================================================ */

void atom_input_state_begin_frame(atom_input_state_t *state, uint64_t frame_time_ms)
{
    if (!state)
        return;

    /* Update timing */
    state->prev_time_ms = state->current_time_ms;
    state->current_time_ms = frame_time_ms;

    if (state->prev_time_ms > 0)
    {
        uint64_t delta_ms = state->current_time_ms - state->prev_time_ms;
        state->delta_time = (float)delta_ms / 1000.0f;

        /* Clamp delta time to prevent large jumps (e.g., when debugging) */
        if (state->delta_time > 0.1f)
        {
            state->delta_time = 0.1f;
        }
    }
    else
    {
        state->delta_time = 0.016f; /* Default ~60 FPS */
    }

    /* Copy current state to previous */
    memcpy(state->keys_prev, state->keys, sizeof(state->keys));
    memcpy(state->mouse.buttons_prev, state->mouse.buttons, sizeof(state->mouse.buttons));

    /* Save previous mouse position */
    state->mouse.prev_x = state->mouse.x;
    state->mouse.prev_y = state->mouse.y;

    /* Reset per-frame accumulators */
    state->mouse.delta_x = 0;
    state->mouse.delta_y = 0;
    state->mouse.scroll_x = 0.0f;
    state->mouse.scroll_y = 0.0f;
    state->mouse.scroll_accum_x = 0.0f;
    state->mouse.scroll_accum_y = 0.0f;
}

void atom_input_state_end_frame(atom_input_state_t *state)
{
    if (!state)
        return;

    /* Finalize scroll values */
    state->mouse.scroll_x = state->mouse.scroll_accum_x;
    state->mouse.scroll_y = state->mouse.scroll_accum_y;
}

/* ============================================================================
 * Event Processing
 * ============================================================================ */

static void update_modifiers_from_key(atom_input_state_t *state, atom_keycode_t key, bool pressed)
{
    uint16_t mod_bit = 0;

    switch (key)
    {
    case ATOM_KEY_LSHIFT:
        mod_bit = ATOM_MOD_LSHIFT;
        break;
    case ATOM_KEY_RSHIFT:
        mod_bit = ATOM_MOD_RSHIFT;
        break;
    case ATOM_KEY_LCTRL:
        mod_bit = ATOM_MOD_LCTRL;
        break;
    case ATOM_KEY_RCTRL:
        mod_bit = ATOM_MOD_RCTRL;
        break;
    case ATOM_KEY_LALT:
        mod_bit = ATOM_MOD_LALT;
        break;
    case ATOM_KEY_RALT:
        mod_bit = ATOM_MOD_RALT;
        break;
    default:
        return;
    }

    if (pressed)
    {
        state->modifiers |= mod_bit;
    }
    else
    {
        state->modifiers &= ~mod_bit;
    }
}

void atom_input_state_process_event(atom_input_state_t *state, const atom_input_event_t *event)
{
    if (!state || !event)
        return;

    switch (event->type)
    {
    case ATOM_EVENT_KEY_DOWN:
        if (event->data.key.key >= 0 && event->data.key.key < ATOM_KEY_COUNT)
        {
            if (!state->keys[event->data.key.key])
            {
                /* Key was just pressed */
                state->key_press_time[event->data.key.key] = event->timestamp_ms;
            }
            state->keys[event->data.key.key] = true;
            update_modifiers_from_key(state, event->data.key.key, true);
        }
        break;

    case ATOM_EVENT_KEY_UP:
        if (event->data.key.key >= 0 && event->data.key.key < ATOM_KEY_COUNT)
        {
            state->keys[event->data.key.key] = false;
            update_modifiers_from_key(state, event->data.key.key, false);
        }
        break;

    case ATOM_EVENT_KEY_REPEAT:
        /* Repeat events don't change state, just for text input */
        break;

    case ATOM_EVENT_MOUSE_BUTTON_DOWN:
        if (event->data.mouse_button.button >= 0 &&
            event->data.mouse_button.button < ATOM_MOUSE_BUTTON_COUNT)
        {
            state->mouse.buttons[event->data.mouse_button.button] = true;
            state->mouse.x = event->data.mouse_button.x;
            state->mouse.y = event->data.mouse_button.y;
        }
        break;

    case ATOM_EVENT_MOUSE_BUTTON_UP:
        if (event->data.mouse_button.button >= 0 &&
            event->data.mouse_button.button < ATOM_MOUSE_BUTTON_COUNT)
        {
            state->mouse.buttons[event->data.mouse_button.button] = false;
            state->mouse.x = event->data.mouse_button.x;
            state->mouse.y = event->data.mouse_button.y;
        }
        break;

    case ATOM_EVENT_MOUSE_MOVE:
        state->mouse.x = event->data.mouse_move.x;
        state->mouse.y = event->data.mouse_move.y;
        state->mouse.delta_x += event->data.mouse_move.delta_x;
        state->mouse.delta_y += event->data.mouse_move.delta_y;
        break;

    case ATOM_EVENT_MOUSE_SCROLL:
        state->mouse.scroll_accum_x += event->data.mouse_scroll.delta_x;
        state->mouse.scroll_accum_y += event->data.mouse_scroll.delta_y;
        break;

    case ATOM_EVENT_WINDOW_FOCUS_GAINED:
        state->window_focused = true;
        break;

    case ATOM_EVENT_WINDOW_FOCUS_LOST:
        state->window_focused = false;
        /* Clear all keys when losing focus to prevent stuck keys */
        memset(state->keys, 0, sizeof(state->keys));
        memset(state->mouse.buttons, 0, sizeof(state->mouse.buttons));
        state->modifiers = ATOM_MOD_NONE;
        break;

    case ATOM_EVENT_WINDOW_RESIZE:
        state->window_width = event->data.window_resize.width;
        state->window_height = event->data.window_resize.height;
        break;

    case ATOM_EVENT_WINDOW_CLOSE:
    default:
        break;
    }
}

/* ============================================================================
 * Keyboard Queries
 * ============================================================================ */

bool atom_key_is_down(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return false;
    return state->keys[key];
}

bool atom_key_is_up(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return true;
    return !state->keys[key];
}

bool atom_key_just_pressed(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return false;
    return state->keys[key] && !state->keys_prev[key];
}

bool atom_key_just_released(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return false;
    return !state->keys[key] && state->keys_prev[key];
}

atom_key_state_t atom_key_get_state(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return ATOM_KEY_STATE_UP;

    bool current = state->keys[key];
    bool previous = state->keys_prev[key];

    if (current && !previous)
        return ATOM_KEY_STATE_PRESSED;
    if (current && previous)
        return ATOM_KEY_STATE_HELD;
    if (!current && previous)
        return ATOM_KEY_STATE_RELEASED;
    return ATOM_KEY_STATE_UP;
}

uint64_t atom_key_held_duration_ms(const atom_input_state_t *state, atom_keycode_t key)
{
    if (!state || key < 0 || key >= ATOM_KEY_COUNT)
        return 0;
    if (!state->keys[key])
        return 0;

    uint64_t press_time = state->key_press_time[key];
    if (press_time == 0)
        return 0;

    return state->current_time_ms - press_time;
}

float atom_key_held_duration_sec(const atom_input_state_t *state, atom_keycode_t key)
{
    return (float)atom_key_held_duration_ms(state, key) / 1000.0f;
}

bool atom_any_key_down(const atom_input_state_t *state)
{
    if (!state)
        return false;
    for (int i = 0; i < ATOM_KEY_COUNT; i++)
    {
        if (state->keys[i])
            return true;
    }
    return false;
}

bool atom_any_key_just_pressed(const atom_input_state_t *state)
{
    if (!state)
        return false;
    for (int i = 0; i < ATOM_KEY_COUNT; i++)
    {
        if (state->keys[i] && !state->keys_prev[i])
            return true;
    }
    return false;
}

/* ============================================================================
 * Mouse Queries
 * ============================================================================ */

void atom_mouse_get_position(const atom_input_state_t *state, int32_t *x, int32_t *y)
{
    if (!state)
    {
        if (x)
            *x = 0;
        if (y)
            *y = 0;
        return;
    }
    if (x)
        *x = state->mouse.x;
    if (y)
        *y = state->mouse.y;
}

atom_vec2_t atom_mouse_get_position_vec(const atom_input_state_t *state)
{
    if (!state)
        return atom_vec2_zero();
    return atom_vec2_create((float)state->mouse.x, (float)state->mouse.y);
}

void atom_mouse_get_delta(const atom_input_state_t *state, int32_t *dx, int32_t *dy)
{
    if (!state)
    {
        if (dx)
            *dx = 0;
        if (dy)
            *dy = 0;
        return;
    }
    if (dx)
        *dx = state->mouse.delta_x;
    if (dy)
        *dy = state->mouse.delta_y;
}

atom_vec2_t atom_mouse_get_delta_vec(const atom_input_state_t *state)
{
    if (!state)
        return atom_vec2_zero();
    return atom_vec2_create((float)state->mouse.delta_x, (float)state->mouse.delta_y);
}

void atom_mouse_get_scroll(const atom_input_state_t *state, float *sx, float *sy)
{
    if (!state)
    {
        if (sx)
            *sx = 0.0f;
        if (sy)
            *sy = 0.0f;
        return;
    }
    if (sx)
        *sx = state->mouse.scroll_x;
    if (sy)
        *sy = state->mouse.scroll_y;
}

atom_vec2_t atom_mouse_get_scroll_vec(const atom_input_state_t *state)
{
    if (!state)
        return atom_vec2_zero();
    return atom_vec2_create(state->mouse.scroll_x, state->mouse.scroll_y);
}

bool atom_mouse_button_is_down(const atom_input_state_t *state, atom_mouse_button_t button)
{
    if (!state || button < 0 || button >= ATOM_MOUSE_BUTTON_COUNT)
        return false;
    return state->mouse.buttons[button];
}

bool atom_mouse_button_is_up(const atom_input_state_t *state, atom_mouse_button_t button)
{
    if (!state || button < 0 || button >= ATOM_MOUSE_BUTTON_COUNT)
        return true;
    return !state->mouse.buttons[button];
}

bool atom_mouse_button_just_pressed(const atom_input_state_t *state, atom_mouse_button_t button)
{
    if (!state || button < 0 || button >= ATOM_MOUSE_BUTTON_COUNT)
        return false;
    return state->mouse.buttons[button] && !state->mouse.buttons_prev[button];
}

bool atom_mouse_button_just_released(const atom_input_state_t *state, atom_mouse_button_t button)
{
    if (!state || button < 0 || button >= ATOM_MOUSE_BUTTON_COUNT)
        return false;
    return !state->mouse.buttons[button] && state->mouse.buttons_prev[button];
}

bool atom_any_mouse_button_down(const atom_input_state_t *state)
{
    if (!state)
        return false;
    for (int i = 0; i < ATOM_MOUSE_BUTTON_COUNT; i++)
    {
        if (state->mouse.buttons[i])
            return true;
    }
    return false;
}

bool atom_any_mouse_button_just_pressed(const atom_input_state_t *state)
{
    if (!state)
        return false;
    for (int i = 0; i < ATOM_MOUSE_BUTTON_COUNT; i++)
    {
        if (state->mouse.buttons[i] && !state->mouse.buttons_prev[i])
            return true;
    }
    return false;
}

/* ============================================================================
 * Modifier Queries
 * ============================================================================ */

bool atom_modifier_is_active(const atom_input_state_t *state, uint16_t modifier)
{
    if (!state)
        return false;
    return (state->modifiers & modifier) != 0;
}

bool atom_shift_down(const atom_input_state_t *state)
{
    return atom_modifier_is_active(state, ATOM_MOD_SHIFT);
}

bool atom_ctrl_down(const atom_input_state_t *state)
{
    return atom_modifier_is_active(state, ATOM_MOD_CTRL);
}

bool atom_alt_down(const atom_input_state_t *state)
{
    return atom_modifier_is_active(state, ATOM_MOD_ALT);
}

/* ============================================================================
 * Movement Helpers
 * ============================================================================ */

atom_vec3_t atom_input_get_movement_vector(const atom_input_state_t *state)
{
    if (!state)
        return atom_vec3_zero();

    atom_vec3_t move = atom_vec3_zero();

    /* Forward/Backward (Z-axis) - Physical WASD positions (ZQSD on AZERTY) */
    if (atom_key_is_down(state, ATOM_KEY_W))
        move.z -= 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_S))
        move.z += 1.0f;

    /* Left/Right (X-axis) - Physical WASD positions (ZQSD on AZERTY) */
    if (atom_key_is_down(state, ATOM_KEY_A))
        move.x -= 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_D))
        move.x += 1.0f;

    /* Normalize to prevent diagonal speed boost */
    float len_sq = atom_vec3_length_sq(move);
    if (len_sq > ATOM_EPSILON)
    {
        move = atom_vec3_scale(move, 1.0f / sqrtf(len_sq));
    }

    return move;
}

float atom_input_get_vertical_movement(const atom_input_state_t *state)
{
    if (!state)
        return 0.0f;

    float vertical = 0.0f;

    if (atom_key_is_down(state, ATOM_KEY_SPACE))
        vertical += 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_LCTRL) || atom_key_is_down(state, ATOM_KEY_RCTRL))
        vertical -= 1.0f;

    return vertical;
}

atom_vec3_t atom_input_get_full_movement_vector(const atom_input_state_t *state)
{
    if (!state)
        return atom_vec3_zero();

    atom_vec3_t move = atom_vec3_zero();

    /* Horizontal */
    if (atom_key_is_down(state, ATOM_KEY_W))
        move.z -= 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_S))
        move.z += 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_A))
        move.x -= 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_D))
        move.x += 1.0f;

    /* Vertical */
    if (atom_key_is_down(state, ATOM_KEY_SPACE))
        move.y += 1.0f;
    if (atom_key_is_down(state, ATOM_KEY_LCTRL) || atom_key_is_down(state, ATOM_KEY_RCTRL))
        move.y -= 1.0f;

    /* Normalize */
    float len_sq = atom_vec3_length_sq(move);
    if (len_sq > ATOM_EPSILON)
    {
        move = atom_vec3_scale(move, 1.0f / sqrtf(len_sq));
    }

    return move;
}

/* ============================================================================
 * Timing
 * ============================================================================ */

float atom_input_state_get_delta_time(const atom_input_state_t *state)
{
    return state ? state->delta_time : 0.016f;
}

uint64_t atom_input_state_get_time_ms(const atom_input_state_t *state)
{
    return state ? state->current_time_ms : 0;
}
