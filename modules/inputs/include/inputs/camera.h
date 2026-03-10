/**
 * @file camera.h
 * @brief Base camera abstraction for ATOM game engine
 *
 * Provides a flexible camera system with support for multiple modes
 * (FPS, Orbital). Uses quaternion-based orientation to avoid gimbal lock.
 */

#ifndef ATOM_CAMERA_H_
#define ATOM_CAMERA_H_

#include "atom_math.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * Camera Mode
     * ============================================================================ */

    typedef enum atom_camera_mode
    {
        ATOM_CAMERA_MODE_FPS = 0,
        ATOM_CAMERA_MODE_ORBITAL,
        ATOM_CAMERA_MODE_COUNT
    } atom_camera_mode_t;

    /* ============================================================================
     * Camera Settings
     * ============================================================================ */

    typedef struct atom_camera_settings
    {
        /* Movement */
        float move_speed;        /* Units per second */
        float sprint_multiplier; /* Speed multiplier when sprinting */

        /* Mouse look */
        float mouse_sensitivity; /* Degrees per pixel */
        bool invert_y;           /* Invert Y-axis mouse look */

        /* Pitch constraints */
        float min_pitch_deg; /* Minimum pitch in degrees (e.g., -89) */
        float max_pitch_deg; /* Maximum pitch in degrees (e.g., +89) */

        /* Orbital specific */
        float orbit_distance;     /* Distance from focal point */
        float min_orbit_distance; /* Minimum zoom distance */
        float max_orbit_distance; /* Maximum zoom distance */
        float zoom_speed;         /* Distance change per scroll unit */
        float pan_speed;          /* Pan speed multiplier */

        /* Smoothing (0 = instant, higher = smoother) */
        float position_smoothing;
        float rotation_smoothing;
    } atom_camera_settings_t;

    /* Get default settings */
    atom_camera_settings_t atom_camera_settings_default(void);

    /* ============================================================================
     * Camera Structure
     * ============================================================================ */

    typedef struct atom_camera
    {
        /* ---- Transform ---- */
        atom_vec3_t position;
        atom_quat_t orientation;

        /* Euler angles (for constraints and UI, derived from orientation) */
        float pitch; /* Radians, constrained */
        float yaw;   /* Radians, unconstrained */

        /* ---- Orbital mode ---- */
        atom_vec3_t focal_point;
        float orbit_distance;

        /* ---- Cached direction vectors ---- */
        atom_vec3_t forward;
        atom_vec3_t right;
        atom_vec3_t up;

        /* ---- Matrices ---- */
        atom_mat4_t view_matrix;
        atom_mat4_t projection_matrix;
        atom_mat4_t view_projection;

        /* ---- Projection parameters ---- */
        float fov_rad;
        float aspect_ratio;
        float near_plane;
        float far_plane;

        /* ---- Settings ---- */
        atom_camera_settings_t settings;
        atom_camera_mode_t mode;

        /* ---- Dirty flags ---- */
        bool view_dirty;
        bool projection_dirty;
        bool vectors_dirty;
    } atom_camera_t;

    /* ============================================================================
     * Camera Lifecycle
     * ============================================================================ */

    /**
     * Initialize camera with default settings
     */
    void atom_camera_init(atom_camera_t *camera, atom_camera_mode_t mode);

    /**
     * Initialize camera with perspective projection
     */
    void atom_camera_init_perspective(atom_camera_t *camera, float fov_degrees, float aspect,
                                      float near_plane, float far_plane);

    /* ============================================================================
     * Transform Setters
     * ============================================================================ */

    void atom_camera_set_position(atom_camera_t *camera, atom_vec3_t position);
    void atom_camera_set_orientation(atom_camera_t *camera, atom_quat_t orientation);

    /**
     * Set camera orientation from Euler angles (degrees)
     */
    void atom_camera_set_euler(atom_camera_t *camera, float pitch_deg, float yaw_deg);

    /**
     * Set camera orientation from Euler angles (radians)
     */
    void atom_camera_set_euler_rad(atom_camera_t *camera, float pitch, float yaw);

    /**
     * Make camera look at a target position
     */
    void atom_camera_look_at(atom_camera_t *camera, atom_vec3_t target);

    /* ============================================================================
     * Orbital Mode
     * ============================================================================ */

    void atom_camera_set_focal_point(atom_camera_t *camera, atom_vec3_t focal_point);
    void atom_camera_set_orbit_distance(atom_camera_t *camera, float distance);

    /* ============================================================================
     * Projection Setters
     * ============================================================================ */

    void atom_camera_set_perspective(atom_camera_t *camera, float fov_degrees, float aspect,
                                     float near_plane, float far_plane);

    void atom_camera_set_fov(atom_camera_t *camera, float fov_degrees);
    void atom_camera_set_aspect(atom_camera_t *camera, float aspect);
    void atom_camera_set_near_far(atom_camera_t *camera, float near_plane, float far_plane);

    /* ============================================================================
     * Matrix Getters (lazy evaluation)
     * ============================================================================ */

    /**
     * Get view matrix (recalculates if dirty)
     */
    const atom_mat4_t *atom_camera_get_view_matrix(atom_camera_t *camera);

    /**
     * Get projection matrix (recalculates if dirty)
     */
    const atom_mat4_t *atom_camera_get_projection_matrix(atom_camera_t *camera);

    /**
     * Get combined view-projection matrix
     */
    const atom_mat4_t *atom_camera_get_view_projection(atom_camera_t *camera);

    /* ============================================================================
     * Direction Vector Getters
     * ============================================================================ */

    atom_vec3_t atom_camera_get_position(const atom_camera_t *camera);
    atom_vec3_t atom_camera_get_forward(atom_camera_t *camera);
    atom_vec3_t atom_camera_get_right(atom_camera_t *camera);
    atom_vec3_t atom_camera_get_up(atom_camera_t *camera);

    /* ============================================================================
     * Mode Control
     * ============================================================================ */

    void atom_camera_set_mode(atom_camera_t *camera, atom_camera_mode_t mode);
    atom_camera_mode_t atom_camera_get_mode(const atom_camera_t *camera);

    /* ============================================================================
     * Internal Update Functions
     * ============================================================================ */

    /**
     * Recalculate direction vectors from orientation (if dirty)
     */
    void atom_camera_update_vectors(atom_camera_t *camera);

    /**
     * Recalculate matrices (if dirty)
     */
    void atom_camera_update_matrices(atom_camera_t *camera);

    /**
     * Force recalculation of all cached values
     */
    void atom_camera_force_update(atom_camera_t *camera);

    /**
     * Rebuild quaternion orientation from Euler angles
     * Call this after modifying pitch/yaw directly
     */
    void atom_camera_rebuild_orientation(atom_camera_t *camera);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_CAMERA_H_ */
