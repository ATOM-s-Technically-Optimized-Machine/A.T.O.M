/**
 * @file camera.c
 * @brief Implementation of base camera system
 */

#include "inputs/camera.h"
#include <string.h>

/* ============================================================================
 * Default Settings
 * ============================================================================ */

atom_camera_settings_t atom_camera_settings_default(void)
{
    atom_camera_settings_t settings = {
        /* Movement */
        .move_speed = 5.0f,
        .sprint_multiplier = 2.0f,

        /* Mouse look */
        .mouse_sensitivity = 0.1f,
        .invert_y = false,

        /* Pitch constraints */
        .min_pitch_deg = -89.0f,
        .max_pitch_deg = 89.0f,

        /* Orbital */
        .orbit_distance = 10.0f,
        .min_orbit_distance = 1.0f,
        .max_orbit_distance = 100.0f,
        .zoom_speed = 2.0f,
        .pan_speed = 0.01f,

        /* Smoothing */
        .position_smoothing = 0.0f,
        .rotation_smoothing = 0.0f
    };
    return settings;
}

/* ============================================================================
 * Initialization
 * ============================================================================ */

void atom_camera_init(atom_camera_t *camera, atom_camera_mode_t mode)
{
    if (!camera) return;

    memset(camera, 0, sizeof(atom_camera_t));

    camera->position = atom_vec3_create(0.0f, 0.0f, 5.0f);
    camera->orientation = atom_quat_identity();
    camera->pitch = 0.0f;
    camera->yaw = 0.0f;

    camera->focal_point = atom_vec3_zero();
    camera->orbit_distance = 10.0f;

    camera->forward = atom_vec3_forward();
    camera->right = atom_vec3_right();
    camera->up = atom_vec3_up();

    camera->view_matrix = atom_mat4_identity();
    camera->projection_matrix = atom_mat4_identity();
    camera->view_projection = atom_mat4_identity();

    camera->fov_rad = 60.0f * ATOM_DEG_TO_RAD;
    camera->aspect_ratio = 16.0f / 9.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 1000.0f;

    camera->settings = atom_camera_settings_default();
    camera->mode = mode;

    camera->view_dirty = true;
    camera->projection_dirty = true;
    camera->vectors_dirty = true;

    /* If orbital mode, calculate initial position from focal point */
    if (mode == ATOM_CAMERA_MODE_ORBITAL) {
        camera->orbit_distance = camera->settings.orbit_distance;
        camera->position = atom_vec3_add(
            camera->focal_point,
            atom_vec3_scale(atom_vec3_back(), camera->orbit_distance)
        );
    }
}

void atom_camera_init_perspective(atom_camera_t *camera,
                                  float fov_degrees,
                                  float aspect,
                                  float near_plane,
                                  float far_plane)
{
    if (!camera) return;

    camera->fov_rad = fov_degrees * ATOM_DEG_TO_RAD;
    camera->aspect_ratio = aspect;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;
    camera->projection_dirty = true;
}

/* ============================================================================
 * Transform Setters
 * ============================================================================ */

void atom_camera_set_position(atom_camera_t *camera, atom_vec3_t position)
{
    if (!camera) return;
    camera->position = position;
    camera->view_dirty = true;
}

void atom_camera_set_orientation(atom_camera_t *camera, atom_quat_t orientation)
{
    if (!camera) return;

    camera->orientation = atom_quat_normalize(orientation);

    /* Extract Euler angles from quaternion */
    float roll;
    atom_quat_to_euler(camera->orientation, &camera->pitch, &camera->yaw, &roll);

    camera->vectors_dirty = true;
    camera->view_dirty = true;
}

void atom_camera_set_euler(atom_camera_t *camera, float pitch_deg, float yaw_deg)
{
    atom_camera_set_euler_rad(camera,
                              pitch_deg * ATOM_DEG_TO_RAD,
                              yaw_deg * ATOM_DEG_TO_RAD);
}

void atom_camera_set_euler_rad(atom_camera_t *camera, float pitch, float yaw)
{
    if (!camera) return;

    /* Constrain pitch */
    float min_pitch = camera->settings.min_pitch_deg * ATOM_DEG_TO_RAD;
    float max_pitch = camera->settings.max_pitch_deg * ATOM_DEG_TO_RAD;
    camera->pitch = atom_clampf(pitch, min_pitch, max_pitch);
    camera->yaw = yaw;

    atom_camera_rebuild_orientation(camera);
}

void atom_camera_look_at(atom_camera_t *camera, atom_vec3_t target)
{
    if (!camera) return;

    atom_vec3_t direction = atom_vec3_normalize(
        atom_vec3_sub(target, camera->position)
    );

    /* Calculate yaw and pitch from direction */
    camera->yaw = atan2f(-direction.x, -direction.z);
    camera->pitch = asinf(direction.y);

    /* Constrain pitch */
    float min_pitch = camera->settings.min_pitch_deg * ATOM_DEG_TO_RAD;
    float max_pitch = camera->settings.max_pitch_deg * ATOM_DEG_TO_RAD;
    camera->pitch = atom_clampf(camera->pitch, min_pitch, max_pitch);

    atom_camera_rebuild_orientation(camera);
}

/* ============================================================================
 * Orbital Mode
 * ============================================================================ */

void atom_camera_set_focal_point(atom_camera_t *camera, atom_vec3_t focal_point)
{
    if (!camera) return;
    camera->focal_point = focal_point;
    camera->view_dirty = true;
}

void atom_camera_set_orbit_distance(atom_camera_t *camera, float distance)
{
    if (!camera) return;

    camera->orbit_distance = atom_clampf(
        distance,
        camera->settings.min_orbit_distance,
        camera->settings.max_orbit_distance
    );
    camera->view_dirty = true;
}

/* ============================================================================
 * Projection Setters
 * ============================================================================ */

void atom_camera_set_perspective(atom_camera_t *camera,
                                 float fov_degrees,
                                 float aspect,
                                 float near_plane,
                                 float far_plane)
{
    if (!camera) return;

    camera->fov_rad = fov_degrees * ATOM_DEG_TO_RAD;
    camera->aspect_ratio = aspect;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;
    camera->projection_dirty = true;
}

void atom_camera_set_fov(atom_camera_t *camera, float fov_degrees)
{
    if (!camera) return;
    camera->fov_rad = fov_degrees * ATOM_DEG_TO_RAD;
    camera->projection_dirty = true;
}

void atom_camera_set_aspect(atom_camera_t *camera, float aspect)
{
    if (!camera) return;
    camera->aspect_ratio = aspect;
    camera->projection_dirty = true;
}

void atom_camera_set_near_far(atom_camera_t *camera, float near_plane, float far_plane)
{
    if (!camera) return;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;
    camera->projection_dirty = true;
}

/* ============================================================================
 * Matrix Getters
 * ============================================================================ */

const atom_mat4_t* atom_camera_get_view_matrix(atom_camera_t *camera)
{
    if (!camera) return NULL;
    atom_camera_update_matrices(camera);
    return &camera->view_matrix;
}

const atom_mat4_t* atom_camera_get_projection_matrix(atom_camera_t *camera)
{
    if (!camera) return NULL;
    atom_camera_update_matrices(camera);
    return &camera->projection_matrix;
}

const atom_mat4_t* atom_camera_get_view_projection(atom_camera_t *camera)
{
    if (!camera) return NULL;
    atom_camera_update_matrices(camera);
    return &camera->view_projection;
}

/* ============================================================================
 * Direction Vector Getters
 * ============================================================================ */

atom_vec3_t atom_camera_get_position(const atom_camera_t *camera)
{
    return camera ? camera->position : atom_vec3_zero();
}

atom_vec3_t atom_camera_get_forward(atom_camera_t *camera)
{
    if (!camera) return atom_vec3_forward();
    atom_camera_update_vectors(camera);
    return camera->forward;
}

atom_vec3_t atom_camera_get_right(atom_camera_t *camera)
{
    if (!camera) return atom_vec3_right();
    atom_camera_update_vectors(camera);
    return camera->right;
}

atom_vec3_t atom_camera_get_up(atom_camera_t *camera)
{
    if (!camera) return atom_vec3_up();
    atom_camera_update_vectors(camera);
    return camera->up;
}

/* ============================================================================
 * Mode Control
 * ============================================================================ */

void atom_camera_set_mode(atom_camera_t *camera, atom_camera_mode_t mode)
{
    if (!camera || camera->mode == mode) return;

    atom_camera_mode_t old_mode = camera->mode;
    camera->mode = mode;

    /* Handle mode transition */
    if (mode == ATOM_CAMERA_MODE_ORBITAL) {
        /* Switching TO orbital: set focal point in front of camera */
        atom_camera_update_vectors(camera);
        camera->focal_point = atom_vec3_add(
            camera->position,
            atom_vec3_scale(camera->forward, camera->orbit_distance)
        );
    } else if (old_mode == ATOM_CAMERA_MODE_ORBITAL) {
        /* Switching FROM orbital: keep current position */
        /* Nothing special needed */
    }

    camera->view_dirty = true;
}

atom_camera_mode_t atom_camera_get_mode(const atom_camera_t *camera)
{
    return camera ? camera->mode : ATOM_CAMERA_MODE_FPS;
}

/* ============================================================================
 * Internal Update Functions
 * ============================================================================ */

void atom_camera_update_vectors(atom_camera_t *camera)
{
    if (!camera || !camera->vectors_dirty) return;

    camera->forward = atom_quat_get_forward(camera->orientation);
    camera->right = atom_quat_get_right(camera->orientation);
    camera->up = atom_quat_get_up(camera->orientation);

    camera->vectors_dirty = false;
}

void atom_camera_update_matrices(atom_camera_t *camera)
{
    if (!camera) return;

    /* Update vectors first if needed */
    atom_camera_update_vectors(camera);

    /* Update projection matrix */
    if (camera->projection_dirty) {
        camera->projection_matrix = atom_mat4_perspective(
            camera->fov_rad,
            camera->aspect_ratio,
            camera->near_plane,
            camera->far_plane
        );
        camera->projection_dirty = false;
    }

    /* Update view matrix */
    if (camera->view_dirty) {
        /* For orbital mode, recalculate position from focal point */
        if (camera->mode == ATOM_CAMERA_MODE_ORBITAL) {
            atom_vec3_t back = atom_vec3_negate(camera->forward);
            camera->position = atom_vec3_add(
                camera->focal_point,
                atom_vec3_scale(back, camera->orbit_distance)
            );
        }

        /* Build view matrix */
        atom_vec3_t target = atom_vec3_add(camera->position, camera->forward);
        camera->view_matrix = atom_mat4_look_at(
            camera->position,
            target,
            camera->up
        );

        camera->view_dirty = false;
    }

    /* Update combined matrix */
    camera->view_projection = atom_mat4_multiply(
        camera->projection_matrix,
        camera->view_matrix
    );
}

void atom_camera_force_update(atom_camera_t *camera)
{
    if (!camera) return;

    camera->vectors_dirty = true;
    camera->view_dirty = true;
    camera->projection_dirty = true;

    atom_camera_update_matrices(camera);
}

void atom_camera_rebuild_orientation(atom_camera_t *camera)
{
    if (!camera) return;

    /*
     * Rebuild quaternion from Euler angles.
     * We use yaw (Y-axis) then pitch (X-axis) order.
     * This prevents gimbal lock accumulation by always
     * reconstructing from constrained Euler angles.
     */
    atom_quat_t quat_yaw = atom_quat_from_axis_angle(atom_vec3_up(), camera->yaw);
    atom_quat_t quat_pitch = atom_quat_from_axis_angle(atom_vec3_right(), camera->pitch);

    /* Apply yaw first (global), then pitch (local) */
    camera->orientation = atom_quat_multiply(quat_yaw, quat_pitch);
    camera->orientation = atom_quat_normalize(camera->orientation);

    camera->vectors_dirty = true;
    camera->view_dirty = true;
}
