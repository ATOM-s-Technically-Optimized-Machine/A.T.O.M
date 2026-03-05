/**
 * @file camera_orbital.c
 * @brief Implementation of orbital camera controller
 */

#include "inputs/camera_orbital.h"

/* ============================================================================
 * Default Bindings
 * ============================================================================ */

atom_orbital_camera_bindings_t atom_orbital_camera_bindings_default(void)
{
    atom_orbital_camera_bindings_t bindings = {
        .orbit_button = ATOM_MOUSE_LEFT,
        .pan_button = ATOM_MOUSE_MIDDLE,
        .reset_key = ATOM_KEY_R,
        .focus_key = ATOM_KEY_F
    };
    return bindings;
}

/* ============================================================================
 * Initialization
 * ============================================================================ */

void atom_orbital_camera_init(atom_camera_t *camera,
                              atom_vec3_t focal_point,
                              float initial_distance)
{
    if (!camera) return;

    atom_camera_init(camera, ATOM_CAMERA_MODE_ORBITAL);

    /* Orbital-specific defaults */
    camera->settings.orbit_distance = initial_distance;
    camera->settings.min_orbit_distance = 0.5f;
    camera->settings.max_orbit_distance = 500.0f;
    camera->settings.zoom_speed = 1.0f;
    camera->settings.pan_speed = 0.005f;
    camera->settings.mouse_sensitivity = 0.3f;
    camera->settings.min_pitch_deg = -89.0f;
    camera->settings.max_pitch_deg = 89.0f;

    /* Set focal point and distance */
    camera->focal_point = focal_point;
    camera->orbit_distance = initial_distance;

    /* Default view: looking from slightly above */
    camera->pitch = -30.0f * ATOM_DEG_TO_RAD;
    camera->yaw = 0.0f;

    /* Rebuild orientation and position */
    atom_camera_rebuild_orientation(camera);
    atom_orbital_camera_update_position(camera);
}

/* ============================================================================
 * Main Update
 * ============================================================================ */

void atom_orbital_camera_update(atom_camera_t *camera,
                                const atom_input_state_t *input,
                                const atom_orbital_camera_bindings_t *bindings,
                                float delta_time)
{
    if (!camera || !input) return;
    (void)delta_time;  /* Not used in orbital camera */

    /* Use default bindings if none provided */
    atom_orbital_camera_bindings_t default_bindings;
    if (!bindings) {
        default_bindings = atom_orbital_camera_bindings_default();
        bindings = &default_bindings;
    }

    /* Handle reset key */
    if (atom_key_just_pressed(input, bindings->reset_key)) {
        atom_orbital_camera_reset(camera,
                                  atom_vec3_zero(),
                                  camera->settings.orbit_distance,
                                  -30.0f,
                                  0.0f);
    }

    /* Process zoom (always active via scroll) */
    atom_orbital_camera_process_zoom(camera, input);

    /* Process orbit and pan (mutually exclusive based on button) */
    if (atom_mouse_button_is_down(input, bindings->pan_button)) {
        atom_orbital_camera_process_pan(camera, input, bindings);
    } else if (atom_mouse_button_is_down(input, bindings->orbit_button)) {
        atom_orbital_camera_process_orbit(camera, input, bindings);
    }
}

/* ============================================================================
 * Orbit Processing
 * ============================================================================ */

void atom_orbital_camera_process_orbit(atom_camera_t *camera,
                                       const atom_input_state_t *input,
                                       const atom_orbital_camera_bindings_t *bindings)
{
    if (!camera || !input || !bindings) return;

    /* Check orbit button is held */
    if (!atom_mouse_button_is_down(input, bindings->orbit_button)) {
        return;
    }

    /* Get mouse delta */
    int32_t delta_x, delta_y;
    atom_mouse_get_delta(input, &delta_x, &delta_y);

    if (delta_x == 0 && delta_y == 0) return;

    /* Convert to rotation angles */
    float sensitivity = camera->settings.mouse_sensitivity * ATOM_DEG_TO_RAD;
    float delta_azimuth = -(float)delta_x * sensitivity;
    float delta_elevation = -(float)delta_y * sensitivity;

    /* Apply Y inversion if enabled */
    if (camera->settings.invert_y) {
        delta_elevation = -delta_elevation;
    }

    atom_orbital_camera_orbit(camera, delta_azimuth, delta_elevation);
}

/* ============================================================================
 * Pan Processing
 * ============================================================================ */

void atom_orbital_camera_process_pan(atom_camera_t *camera,
                                     const atom_input_state_t *input,
                                     const atom_orbital_camera_bindings_t *bindings)
{
    if (!camera || !input || !bindings) return;

    /* Check pan button is held */
    if (!atom_mouse_button_is_down(input, bindings->pan_button)) {
        return;
    }

    /* Get mouse delta */
    int32_t delta_x, delta_y;
    atom_mouse_get_delta(input, &delta_x, &delta_y);

    if (delta_x == 0 && delta_y == 0) return;

    /* Convert to pan movement */
    float pan_scale = camera->settings.pan_speed * camera->orbit_distance;
    atom_vec2_t pan_delta = {
        -(float)delta_x * pan_scale,
        (float)delta_y * pan_scale
    };

    atom_orbital_camera_pan(camera, pan_delta);
}

/* ============================================================================
 * Zoom Processing
 * ============================================================================ */

void atom_orbital_camera_process_zoom(atom_camera_t *camera,
                                      const atom_input_state_t *input)
{
    if (!camera || !input) return;

    /* Get scroll wheel delta */
    float scroll_x, scroll_y;
    atom_mouse_get_scroll(input, &scroll_x, &scroll_y);
    (void)scroll_x;  /* Horizontal scroll not used */

    if (atom_absf(scroll_y) < ATOM_EPSILON) return;

    /* Zoom speed scales with distance for consistent feel */
    float zoom_delta = -scroll_y * camera->settings.zoom_speed *
                       (camera->orbit_distance * 0.1f);

    atom_orbital_camera_zoom(camera, zoom_delta);
}

/* ============================================================================
 * Direct Control Functions
 * ============================================================================ */

void atom_orbital_camera_orbit(atom_camera_t *camera,
                               float delta_azimuth,
                               float delta_elevation)
{
    if (!camera) return;

    /* Update Euler angles */
    camera->yaw += delta_azimuth;
    camera->pitch += delta_elevation;

    /* Constrain pitch */
    float min_pitch = camera->settings.min_pitch_deg * ATOM_DEG_TO_RAD;
    float max_pitch = camera->settings.max_pitch_deg * ATOM_DEG_TO_RAD;
    camera->pitch = atom_clampf(camera->pitch, min_pitch, max_pitch);

    /* Wrap yaw */
    camera->yaw = atom_wrap_angle(camera->yaw);

    /* Rebuild orientation and position */
    atom_camera_rebuild_orientation(camera);
    atom_orbital_camera_update_position(camera);
}

void atom_orbital_camera_orbit_deg(atom_camera_t *camera,
                                   float delta_azimuth_deg,
                                   float delta_elevation_deg)
{
    atom_orbital_camera_orbit(camera,
                              delta_azimuth_deg * ATOM_DEG_TO_RAD,
                              delta_elevation_deg * ATOM_DEG_TO_RAD);
}

void atom_orbital_camera_pan(atom_camera_t *camera, atom_vec2_t delta)
{
    if (!camera) return;

    /* Ensure vectors are up to date */
    atom_camera_update_vectors(camera);

    /* Move focal point in camera-relative direction */
    atom_vec3_t pan_movement = atom_vec3_zero();

    /* X = right direction */
    pan_movement = atom_vec3_add(pan_movement,
        atom_vec3_scale(camera->right, delta.x));

    /* Y = up direction */
    pan_movement = atom_vec3_add(pan_movement,
        atom_vec3_scale(camera->up, delta.y));

    camera->focal_point = atom_vec3_add(camera->focal_point, pan_movement);

    /* Update camera position to maintain same view */
    atom_orbital_camera_update_position(camera);
}

void atom_orbital_camera_zoom(atom_camera_t *camera, float delta_distance)
{
    if (!camera) return;

    float new_distance = camera->orbit_distance + delta_distance;

    /* Clamp to valid range */
    new_distance = atom_clampf(new_distance,
                               camera->settings.min_orbit_distance,
                               camera->settings.max_orbit_distance);

    if (atom_absf(new_distance - camera->orbit_distance) < ATOM_EPSILON) {
        return;  /* No change */
    }

    camera->orbit_distance = new_distance;

    /* Update position */
    atom_orbital_camera_update_position(camera);
}

void atom_orbital_camera_set_target(atom_camera_t *camera, atom_vec3_t target)
{
    if (!camera) return;

    camera->focal_point = target;
    atom_orbital_camera_update_position(camera);
}

void atom_orbital_camera_reset(atom_camera_t *camera,
                               atom_vec3_t focal_point,
                               float distance,
                               float pitch_deg,
                               float yaw_deg)
{
    if (!camera) return;

    camera->focal_point = focal_point;
    camera->orbit_distance = atom_clampf(distance,
                                         camera->settings.min_orbit_distance,
                                         camera->settings.max_orbit_distance);

    /* Set Euler angles */
    float min_pitch = camera->settings.min_pitch_deg * ATOM_DEG_TO_RAD;
    float max_pitch = camera->settings.max_pitch_deg * ATOM_DEG_TO_RAD;
    camera->pitch = atom_clampf(pitch_deg * ATOM_DEG_TO_RAD, min_pitch, max_pitch);
    camera->yaw = yaw_deg * ATOM_DEG_TO_RAD;

    /* Rebuild orientation and position */
    atom_camera_rebuild_orientation(camera);
    atom_orbital_camera_update_position(camera);
}

void atom_orbital_camera_update_position(atom_camera_t *camera)
{
    if (!camera) return;

    /* Ensure orientation vectors are current */
    atom_camera_update_vectors(camera);

    /* Position = focal_point - forward * distance */
    /* (forward points AWAY from where we look, so we go backwards from focal) */
    atom_vec3_t offset = atom_vec3_scale(camera->forward, -camera->orbit_distance);
    camera->position = atom_vec3_add(camera->focal_point, offset);

    camera->view_dirty = true;
}
