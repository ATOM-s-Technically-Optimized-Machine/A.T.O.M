/**
 * @file camera_fps.c
 * @brief Implementation of FPS camera controller
 */

#include "inputs/camera_fps.h"

/* ============================================================================
 * Default Bindings
 * ============================================================================ */

atom_fps_camera_bindings_t atom_fps_camera_bindings_default(void)
{
    atom_fps_camera_bindings_t bindings = {
        .forward = ATOM_KEY_W,
        .backward = ATOM_KEY_S,
        .left = ATOM_KEY_A,
        .right = ATOM_KEY_D,
        .up = ATOM_KEY_SPACE,
        .down = ATOM_KEY_LCTRL,
        .sprint = ATOM_KEY_LSHIFT,
        .look_button = ATOM_MOUSE_NONE /* Always active */
    };
    return bindings;
}

/* ============================================================================
 * Initialization
 * ============================================================================ */

void atom_fps_camera_init(atom_camera_t *camera)
{
    if (!camera)
        return;

    atom_camera_init(camera, ATOM_CAMERA_MODE_FPS);

    /* FPS-specific defaults */
    camera->settings.move_speed = 5.0f;
    camera->settings.sprint_multiplier = 2.5f;
    camera->settings.mouse_sensitivity = 0.1f;
    camera->settings.min_pitch_deg = -89.0f;
    camera->settings.max_pitch_deg = 89.0f;
}

/* ============================================================================
 * Main Update
 * ============================================================================ */

void atom_fps_camera_update(atom_camera_t *camera, const atom_input_state_t *input,
                            const atom_fps_camera_bindings_t *bindings, float delta_time)
{
    if (!camera || !input)
        return;

    /* Use default bindings if none provided */
    atom_fps_camera_bindings_t default_bindings;
    if (!bindings)
    {
        default_bindings = atom_fps_camera_bindings_default();
        bindings = &default_bindings;
    }

    /* Process mouse first (rotation) */
    atom_fps_camera_process_mouse(camera, input, bindings);

    /* Then keyboard (movement) */
    atom_fps_camera_process_keyboard(camera, input, bindings, delta_time);
}

/* ============================================================================
 * Keyboard Processing
 * ============================================================================ */

void atom_fps_camera_process_keyboard(atom_camera_t *camera, const atom_input_state_t *input,
                                      const atom_fps_camera_bindings_t *bindings, float delta_time)
{
    if (!camera || !input || !bindings)
        return;

    /* Build movement vector from key states */
    atom_vec3_t move = atom_vec3_zero();

    /* Forward/Backward */
    if (atom_key_is_down(input, bindings->forward))
        move.z -= 1.0f;
    if (atom_key_is_down(input, bindings->backward))
        move.z += 1.0f;

    /* Left/Right (strafe) */
    if (atom_key_is_down(input, bindings->left))
        move.x -= 1.0f;
    if (atom_key_is_down(input, bindings->right))
        move.x += 1.0f;

    /* Up/Down (fly mode) */
    if (atom_key_is_down(input, bindings->up))
        move.y += 1.0f;
    if (atom_key_is_down(input, bindings->down))
        move.y -= 1.0f;

    /* Early exit if no movement */
    float len_sq = atom_vec3_length_sq(move);
    if (len_sq < ATOM_EPSILON)
        return;

    /* Normalize to prevent diagonal speed boost */
    move = atom_vec3_scale(move, 1.0f / sqrtf(len_sq));

    /* Calculate speed */
    float speed = camera->settings.move_speed;
    if (atom_key_is_down(input, bindings->sprint))
    {
        speed *= camera->settings.sprint_multiplier;
    }

    /* Move in local camera space */
    atom_fps_camera_move_local(camera, move, speed * delta_time);
}

/* ============================================================================
 * Mouse Processing
 * ============================================================================ */

void atom_fps_camera_process_mouse(atom_camera_t *camera, const atom_input_state_t *input,
                                   const atom_fps_camera_bindings_t *bindings)
{
    if (!camera || !input || !bindings)
        return;

    /* Check if look button is required */
    if ((int)bindings->look_button >= 0)
    {
        if (!atom_mouse_button_is_down(input, bindings->look_button))
        {
            return; /* Look button not held */
        }
    }

    /* Get mouse delta */
    int32_t delta_x, delta_y;
    atom_mouse_get_delta(input, &delta_x, &delta_y);

    /* Skip if no movement */
    if (delta_x == 0 && delta_y == 0)
        return;

    /* Convert to rotation angles */
    float sensitivity = camera->settings.mouse_sensitivity * ATOM_DEG_TO_RAD;

    float delta_yaw = -(float)delta_x * sensitivity;
    float delta_pitch = -(float)delta_y * sensitivity;

    /* Apply Y inversion if enabled */
    if (camera->settings.invert_y)
    {
        delta_pitch = -delta_pitch;
    }

    /* Apply rotation */
    atom_fps_camera_rotate(camera, delta_pitch, delta_yaw);
}

/* ============================================================================
 * Direct Control Functions
 * ============================================================================ */

void atom_fps_camera_move(atom_camera_t *camera, atom_vec3_t direction, float distance)
{
    if (!camera)
        return;

    float len = atom_vec3_length(direction);
    if (len < ATOM_EPSILON)
        return;

    /* Normalize direction and apply distance */
    atom_vec3_t move = atom_vec3_scale(direction, distance / len);

    camera->position = atom_vec3_add(camera->position, move);
    camera->view_dirty = true;
}

void atom_fps_camera_move_local(atom_camera_t *camera, atom_vec3_t local_direction, float distance)
{
    if (!camera)
        return;

    /* Ensure vectors are up to date */
    atom_camera_update_vectors(camera);

    /* Convert local direction to world direction */
    atom_vec3_t world_move = atom_vec3_zero();

    /* X = right */
    world_move = atom_vec3_add(world_move, atom_vec3_scale(camera->right, local_direction.x));

    /* Y = up */
    world_move = atom_vec3_add(world_move, atom_vec3_scale(camera->up, local_direction.y));

    /* Z = back (negative forward) */
    world_move = atom_vec3_add(world_move, atom_vec3_scale(camera->forward, -local_direction.z));

    /* Apply movement */
    camera->position = atom_vec3_add(camera->position, atom_vec3_scale(world_move, distance));
    camera->view_dirty = true;
}

void atom_fps_camera_rotate(atom_camera_t *camera, float delta_pitch, float delta_yaw)
{
    if (!camera)
        return;

    /* Update Euler angles */
    camera->yaw += delta_yaw;
    camera->pitch += delta_pitch;

    /* Constrain pitch to prevent camera flip */
    float min_pitch = camera->settings.min_pitch_deg * ATOM_DEG_TO_RAD;
    float max_pitch = camera->settings.max_pitch_deg * ATOM_DEG_TO_RAD;
    camera->pitch = atom_clampf(camera->pitch, min_pitch, max_pitch);

    /* Wrap yaw to prevent float overflow over long sessions */
    camera->yaw = atom_wrap_angle(camera->yaw);

    /* Rebuild quaternion from constrained Euler angles */
    atom_camera_rebuild_orientation(camera);
}

void atom_fps_camera_rotate_deg(atom_camera_t *camera, float delta_pitch_deg, float delta_yaw_deg)
{
    atom_fps_camera_rotate(camera, delta_pitch_deg * ATOM_DEG_TO_RAD,
                           delta_yaw_deg * ATOM_DEG_TO_RAD);
}
