/**
 * @file inputs.h
 * @brief Main public API for ATOM Input Module
 *
 * This is the main header to include for using the ATOM input system.
 * It provides:
 * - Event-driven input handling (keyboard, mouse)
 * - Input state management
 * - FPS/Free-fly camera controller
 * - Orbital camera controller
 * - Math types (Vec3, Mat4, Quaternion)
 *
 * This module is backend-agnostic. Events must be pushed externally
 * (e.g., by a window/platform module) via atom_event_queue_push().
 */

#ifndef INPUTS_H_
#define INPUTS_H_

/* Include all sub-modules */
#include "atom_math.h"
#include "camera.h"
#include "camera_fps.h"
#include "camera_orbital.h"
#include "input_event.h"
#include "input_state.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * Input System Context
     * ============================================================================ */

    /**
     * Main input system structure.
     * Manages event queue, listeners, and input state.
     */
    typedef struct atom_input_system
    {
        /* Event management */
        atom_event_queue_t event_queue;
        atom_listener_manager_t listeners;

        /* Current input state */
        atom_input_state_t state;

        /* Timing */
        uint64_t init_time_ms;
        uint64_t current_time_ms;

        /* State flags */
        bool initialized;
        bool mouse_captured;
        bool should_quit;
    } atom_input_system_t;

    /* ============================================================================
     * Input System Lifecycle
     * ============================================================================ */

    /**
     * Initialize the input system
     *
     * @param system Input system to initialize
     * @return true on success, false on failure
     */
    bool atom_input_init(atom_input_system_t *system);

    /**
     * Shutdown the input system and release resources
     */
    void atom_input_shutdown(atom_input_system_t *system);

    /* ============================================================================
     * Per-Frame Update Functions
     * ============================================================================ */

    /**
     * Process queued events: dispatch to listeners and update state.
     * Events must be pushed to system->event_queue externally before calling this.
     */
    void atom_input_process_events(atom_input_system_t *system);

    /**
     * Begin a new input frame.
     * Updates timing and prepares state for new frame.
     *
     * @param system Input system
     * @param frame_time_ms Current time in milliseconds (provided by platform)
     */
    void atom_input_begin_frame(atom_input_system_t *system, uint64_t frame_time_ms);

    /**
     * End the current input frame.
     * Finalizes state for the frame.
     */
    void atom_input_end_frame(atom_input_system_t *system);

    /**
     * Convenience function: begin_frame + process_events in one call.
     * Events must be pushed to the queue before calling this.
     *
     * @param system Input system
     * @param frame_time_ms Current time in milliseconds (provided by platform)
     */
    void atom_input_update(atom_input_system_t *system, uint64_t frame_time_ms);

    /* ============================================================================
     * Mouse Capture (Relative Mode)
     * ============================================================================ */

    /**
     * Set mouse capture state.
     * This only updates the internal flag. The platform/window module
     * is responsible for actually enabling relative mouse mode.
     *
     * @param system Input system
     * @param capture true to capture, false to release
     */
    void atom_input_capture_mouse(atom_input_system_t *system, bool capture);

    /**
     * Check if mouse is currently captured
     */
    bool atom_input_is_mouse_captured(const atom_input_system_t *system);

    /* ============================================================================
     * Event Listeners
     * ============================================================================ */

    /**
     * Add an event listener callback
     *
     * @param system Input system
     * @param type Event type to listen for
     * @param callback Function to call when event occurs
     * @param user_data User data passed to callback
     * @return true on success, false if no listener slots available
     */
    bool atom_input_add_listener(atom_input_system_t *system, atom_input_event_type_t type,
                                 atom_event_callback_t callback, void *user_data);

    /**
     * Remove an event listener
     *
     * @param system Input system
     * @param callback Callback to remove
     * @return true if listener was found and removed
     */
    bool atom_input_remove_listener(atom_input_system_t *system, atom_event_callback_t callback);

    /* ============================================================================
     * State Access
     * ============================================================================ */

    /**
     * Get current input state (read-only)
     */
    const atom_input_state_t *atom_input_get_state(const atom_input_system_t *system);

    /**
     * Get time elapsed since last frame (seconds)
     */
    float atom_input_get_delta_time(const atom_input_system_t *system);

    /**
     * Get time since input system initialization (milliseconds)
     */
    uint64_t atom_input_get_time_ms(const atom_input_system_t *system);

    /**
     * Check if application should quit (window close or quit event)
     */
    bool atom_input_should_quit(const atom_input_system_t *system);

#ifdef __cplusplus
}
#endif

#endif /* !INPUTS_H_ */
