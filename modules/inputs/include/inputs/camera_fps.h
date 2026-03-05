/**
 * @file camera_fps.h
 * @brief FPS/Free-fly camera controller for ATOM game engine
 *
 * Implements first-person style camera movement:
 * - WASD for movement (normalized diagonal)
 * - Mouse for looking around
 * - Space/Ctrl for vertical movement
 * - Shift for sprint
 * - Pitch constrained to prevent camera flip
 */

#ifndef ATOM_CAMERA_FPS_H_
#define ATOM_CAMERA_FPS_H_

#include "camera.h"
#include "input_state.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * FPS Camera Key Bindings
 * ============================================================================ */

typedef struct atom_fps_camera_bindings {
    /* Movement keys */
    atom_keycode_t forward;
    atom_keycode_t backward;
    atom_keycode_t left;
    atom_keycode_t right;
    atom_keycode_t up;
    atom_keycode_t down;

    /* Modifiers */
    atom_keycode_t sprint;

    /* Mouse look activation (-1 for always active) */
    atom_mouse_button_t look_button;
} atom_fps_camera_bindings_t;

/**
 * Get default WASD key bindings
 */
atom_fps_camera_bindings_t atom_fps_camera_bindings_default(void);

/* ============================================================================
 * FPS Camera API
 * ============================================================================ */

/**
 * Initialize camera for FPS mode with default settings
 */
void atom_fps_camera_init(atom_camera_t *camera);

/**
 * Full update: processes keyboard and mouse input
 *
 * @param camera Camera to update
 * @param input Current input state
 * @param bindings Key bindings (NULL for defaults)
 * @param delta_time Time since last frame in seconds
 */
void atom_fps_camera_update(atom_camera_t *camera,
                            const atom_input_state_t *input,
                            const atom_fps_camera_bindings_t *bindings,
                            float delta_time);

/* ============================================================================
 * Individual Update Functions (for custom control)
 * ============================================================================ */

/**
 * Process keyboard input for movement
 */
void atom_fps_camera_process_keyboard(atom_camera_t *camera,
                                      const atom_input_state_t *input,
                                      const atom_fps_camera_bindings_t *bindings,
                                      float delta_time);

/**
 * Process mouse input for rotation
 */
void atom_fps_camera_process_mouse(atom_camera_t *camera,
                                   const atom_input_state_t *input,
                                   const atom_fps_camera_bindings_t *bindings);

/* ============================================================================
 * Direct Control Functions
 * ============================================================================ */

/**
 * Move camera in a direction (world space)
 *
 * @param camera Camera to move
 * @param direction Movement direction (will be normalized)
 * @param distance Distance to move
 */
void atom_fps_camera_move(atom_camera_t *camera, atom_vec3_t direction, float distance);

/**
 * Move camera relative to its orientation
 *
 * @param camera Camera to move
 * @param local_direction Direction in camera space (forward = -Z, right = +X, up = +Y)
 * @param distance Distance to move
 */
void atom_fps_camera_move_local(atom_camera_t *camera, atom_vec3_t local_direction, float distance);

/**
 * Rotate camera by delta angles
 *
 * @param camera Camera to rotate
 * @param delta_pitch Pitch change in radians (positive = look up)
 * @param delta_yaw Yaw change in radians (positive = look left)
 */
void atom_fps_camera_rotate(atom_camera_t *camera, float delta_pitch, float delta_yaw);

/**
 * Rotate camera by delta angles (degrees)
 */
void atom_fps_camera_rotate_deg(atom_camera_t *camera, float delta_pitch_deg, float delta_yaw_deg);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_CAMERA_FPS_H_ */
