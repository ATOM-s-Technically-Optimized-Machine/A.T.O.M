/**
 * @file inputs.c
 * @brief Main input system implementation (backend-agnostic)
 *
 * This file contains no SDL or platform-specific code.
 * Events are pushed externally via atom_event_queue_push().
 * Timing is provided externally via atom_input_begin_frame().
 */

#include "inputs/inputs.h"

#include <string.h>

/* ============================================================================
 * Input System Lifecycle
 * ============================================================================ */

bool atom_input_init(atom_input_system_t *system)
{
    if (!system)
        return false;

    memset(system, 0, sizeof(atom_input_system_t));

    /* Initialize sub-systems */
    atom_event_queue_init(&system->event_queue);
    atom_listener_manager_init(&system->listeners);
    atom_input_state_init(&system->state);

    system->init_time_ms = 0;
    system->current_time_ms = 0;

    system->initialized = true;
    system->mouse_captured = false;
    system->should_quit = false;

    return true;
}

void atom_input_shutdown(atom_input_system_t *system)
{
    if (!system || !system->initialized)
        return;

    /* Release mouse if captured */
    if (system->mouse_captured)
    {
        atom_input_capture_mouse(system, false);
    }

    /* Clear everything */
    atom_event_queue_clear(&system->event_queue);
    atom_listener_manager_clear(&system->listeners);
    atom_input_state_reset(&system->state);

    system->initialized = false;
}

/* ============================================================================
 * Per-Frame Update Functions
 * ============================================================================ */

void atom_input_process_events(atom_input_system_t *system)
{
    if (!system || !system->initialized)
        return;

    atom_input_event_t event;

    while (atom_event_queue_pop(&system->event_queue, &event))
    {
        /* Check for quit event */
        if (event.type == ATOM_EVENT_WINDOW_CLOSE)
        {
            system->should_quit = true;
        }

        /* Dispatch to listeners */
        atom_listener_dispatch(&system->listeners, &event);

        /* Update state */
        atom_input_state_process_event(&system->state, &event);
    }
}

void atom_input_begin_frame(atom_input_system_t *system, uint64_t frame_time_ms)
{
    if (!system || !system->initialized)
        return;

    if (system->init_time_ms == 0)
    {
        system->init_time_ms = frame_time_ms;
    }

    system->current_time_ms = frame_time_ms - system->init_time_ms;
    atom_input_state_begin_frame(&system->state, system->current_time_ms);
}

void atom_input_end_frame(atom_input_system_t *system)
{
    if (!system || !system->initialized)
        return;

    atom_input_state_end_frame(&system->state);
}

void atom_input_update(atom_input_system_t *system, uint64_t frame_time_ms)
{
    if (!system || !system->initialized)
        return;

    atom_input_begin_frame(system, frame_time_ms);
    atom_input_process_events(system);
}

/* ============================================================================
 * Mouse Capture
 * ============================================================================ */

void atom_input_capture_mouse(atom_input_system_t *system, bool capture)
{
    if (!system || !system->initialized)
        return;

    system->mouse_captured = capture;
    system->state.mouse.is_captured = capture;
}

bool atom_input_is_mouse_captured(const atom_input_system_t *system)
{
    return system ? system->mouse_captured : false;
}

/* ============================================================================
 * Event Listeners
 * ============================================================================ */

bool atom_input_add_listener(atom_input_system_t *system, atom_input_event_type_t type,
                             atom_event_callback_t callback, void *user_data)
{
    if (!system || !system->initialized || !callback)
        return false;

    return atom_listener_add(&system->listeners, type, callback, user_data);
}

bool atom_input_remove_listener(atom_input_system_t *system, atom_event_callback_t callback)
{
    if (!system || !system->initialized || !callback)
        return false;

    return atom_listener_remove(&system->listeners, callback);
}

/* ============================================================================
 * State Access
 * ============================================================================ */

const atom_input_state_t *atom_input_get_state(const atom_input_system_t *system)
{
    return system ? &system->state : NULL;
}

float atom_input_get_delta_time(const atom_input_system_t *system)
{
    return system ? system->state.delta_time : 0.016f;
}

uint64_t atom_input_get_time_ms(const atom_input_system_t *system)
{
    return system ? system->current_time_ms : 0;
}

bool atom_input_should_quit(const atom_input_system_t *system)
{
    return system ? system->should_quit : true;
}
