/**
 * @file camera_orbital.h
 * @brief Orbital camera controller for ATOM game engine
 *
 * Implements orbit-style camera around a focal point:
 * - Left mouse drag to orbit around focal point
 * - Middle mouse drag to pan focal point
 * - Scroll wheel to zoom (change distance)
 * - Suitable for 3D editors or object inspection
 */

#ifndef ATOM_CAMERA_ORBITAL_H_
#define ATOM_CAMERA_ORBITAL_H_

#include "camera.h"
#include "input_state.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Orbital Camera Key Bindings
 * ============================================================================ */

typedef struct atom_orbital_camera_bindings {
    /* Mouse buttons for camera control */
    atom_mouse_button_t orbit_button;   /* Button to orbit around focal point */
    atom_mouse_button_t pan_button;     /* Button to pan focal point */

    /* Keyboard shortcuts */
    atom_keycode_t reset_key;           /* Key to reset view */
    atom_keycode_t focus_key;           /* Key to focus on selection (reserved) */
} atom_orbital_camera_bindings_t;

/**
 * Get default orbital camera bindings
 */
atom_orbital_camera_bindings_t atom_orbital_camera_bindings_default(void);

/* ============================================================================
 * Orbital Camera API
 * ============================================================================ */

/**
 * Initialize camera for orbital mode
 *
 * @param camera Camera to initialize
 * @param focal_point Initial point to orbit around
 * @param initial_distance Initial distance from focal point
 */
void atom_orbital_camera_init(atom_camera_t *camera,
                              atom_vec3_t focal_point,
                              float initial_distance);

/**
 * Full update: processes orbit, pan, and zoom input
 *
 * @param camera Camera to update
 * @param input Current input state
 * @param bindings Key bindings (NULL for defaults)
 * @param delta_time Time since last frame in seconds
 */
void atom_orbital_camera_update(atom_camera_t *camera,
                                const atom_input_state_t *input,
                                const atom_orbital_camera_bindings_t *bindings,
                                float delta_time);

/* ============================================================================
 * Individual Update Functions
 * ============================================================================ */

/**
 * Process orbit input (rotation around focal point)
 */
void atom_orbital_camera_process_orbit(atom_camera_t *camera,
                                       const atom_input_state_t *input,
                                       const atom_orbital_camera_bindings_t *bindings);

/**
 * Process pan input (move focal point)
 */
void atom_orbital_camera_process_pan(atom_camera_t *camera,
                                     const atom_input_state_t *input,
                                     const atom_orbital_camera_bindings_t *bindings);

/**
 * Process zoom input (change distance)
 */
void atom_orbital_camera_process_zoom(atom_camera_t *camera,
                                      const atom_input_state_t *input);

/* ============================================================================
 * Direct Control Functions
 * ============================================================================ */

/**
 * Orbit camera around focal point
 *
 * @param camera Camera to orbit
 * @param delta_azimuth Horizontal rotation in radians
 * @param delta_elevation Vertical rotation in radians
 */
void atom_orbital_camera_orbit(atom_camera_t *camera,
                               float delta_azimuth,
                               float delta_elevation);

/**
 * Orbit camera (degrees)
 */
void atom_orbital_camera_orbit_deg(atom_camera_t *camera,
                                   float delta_azimuth_deg,
                                   float delta_elevation_deg);

/**
 * Pan focal point (move it in camera-relative direction)
 *
 * @param camera Camera
 * @param delta Movement in camera space (X = right, Y = up)
 */
void atom_orbital_camera_pan(atom_camera_t *camera, atom_vec2_t delta);

/**
 * Zoom (change distance from focal point)
 *
 * @param camera Camera
 * @param delta_distance Distance change (positive = zoom out)
 */
void atom_orbital_camera_zoom(atom_camera_t *camera, float delta_distance);

/**
 * Set focal point directly
 */
void atom_orbital_camera_set_target(atom_camera_t *camera, atom_vec3_t target);

/**
 * Reset camera to specific view
 *
 * @param camera Camera to reset
 * @param focal_point New focal point
 * @param distance New distance
 * @param pitch_deg New pitch angle in degrees
 * @param yaw_deg New yaw angle in degrees
 */
void atom_orbital_camera_reset(atom_camera_t *camera,
                               atom_vec3_t focal_point,
                               float distance,
                               float pitch_deg,
                               float yaw_deg);

/**
 * Recalculate camera position from focal point + orientation + distance
 */
void atom_orbital_camera_update_position(atom_camera_t *camera);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_CAMERA_ORBITAL_H_ */
