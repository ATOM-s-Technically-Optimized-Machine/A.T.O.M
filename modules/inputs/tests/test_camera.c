/**
 * @file test_camera.c
 * @brief Unit tests for camera system: base, FPS, orbital
 */

#include "test_helpers.h"
#include <inputs/inputs.h>

/* ========================================================================== *
 * Base Camera Tests
 * ========================================================================== */

static void test_camera_init_fps(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);

    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_FPS);
}

static void test_camera_init_orbital(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_ORBITAL);
    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_ORBITAL);
}

static void test_camera_set_position(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);

    atom_vec3_t new_pos = atom_vec3_create(10, 20, 30);
    atom_camera_set_position(&cam, new_pos);

    atom_vec3_t pos = atom_camera_get_position(&cam);
    ASSERT_VEC3_EQ(pos, new_pos);
}

static void test_camera_dirty_flags(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_force_update(&cam);

    /* After force update, flags should be clean */
    ASSERT_FALSE(cam.view_dirty);
    ASSERT_FALSE(cam.vectors_dirty);

    /* Setting position should dirty the view */
    atom_camera_set_position(&cam, atom_vec3_create(1, 0, 0));
    ASSERT_TRUE(cam.view_dirty);
}

static void test_camera_perspective(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_perspective(&cam, 60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

    const atom_mat4_t *proj = atom_camera_get_projection_matrix(&cam);
    /* Projection matrix should be non-zero */
    ASSERT_TRUE(proj->m[0] != 0.0f);
    ASSERT_TRUE(proj->m[5] != 0.0f);
}

static void test_camera_set_fov(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_perspective(&cam, 60.0f, 1.0f, 0.1f, 100.0f);
    atom_camera_force_update(&cam);

    atom_camera_set_fov(&cam, 90.0f);
    ASSERT_TRUE(cam.projection_dirty);
}

static void test_camera_set_aspect(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_perspective(&cam, 60.0f, 1.0f, 0.1f, 100.0f);
    atom_camera_force_update(&cam);

    atom_camera_set_aspect(&cam, 2.0f);
    ASSERT_TRUE(cam.projection_dirty);
}

static void test_camera_view_matrix(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_position(&cam, atom_vec3_create(0, 0, 5));
    atom_camera_set_perspective(&cam, 60.0f, 1.0f, 0.1f, 100.0f);

    const atom_mat4_t *view = atom_camera_get_view_matrix(&cam);
    /* View matrix transforms eye position to origin */
    atom_vec3_t origin = atom_mat4_transform_point(*view, atom_vec3_create(0, 0, 5));
    ASSERT_VEC3_NEAR(origin, atom_vec3_zero(), 1e-3f);
}

static void test_camera_direction_vectors(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);

    /* Default orientation: forward = (0,0,-1) */
    atom_vec3_t fwd = atom_camera_get_forward(&cam);
    atom_vec3_t rgt = atom_camera_get_right(&cam);
    atom_vec3_t up = atom_camera_get_up(&cam);

    ASSERT_VEC3_NEAR(fwd, atom_vec3_create(0, 0, -1), 1e-3f);
    ASSERT_VEC3_NEAR(rgt, atom_vec3_create(1, 0, 0), 1e-3f);
    ASSERT_VEC3_NEAR(up, atom_vec3_create(0, 1, 0), 1e-3f);
}

static void test_camera_set_euler(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);

    /* Look 90 degrees right (yaw = -90 deg) */
    atom_camera_set_euler(&cam, 0.0f, -90.0f);
    atom_vec3_t fwd = atom_camera_get_forward(&cam);

    /* Should now face +X */
    ASSERT_VEC3_NEAR(fwd, atom_vec3_create(1, 0, 0), 0.05f);
}

static void test_camera_look_at(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_position(&cam, atom_vec3_create(0, 0, 10));

    atom_camera_look_at(&cam, atom_vec3_zero());
    atom_vec3_t fwd = atom_camera_get_forward(&cam);

    /* Should face -Z (towards origin) */
    ASSERT_VEC3_NEAR(fwd, atom_vec3_create(0, 0, -1), 0.05f);
}

static void test_camera_mode_switch(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_FPS);

    atom_camera_set_mode(&cam, ATOM_CAMERA_MODE_ORBITAL);
    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_ORBITAL);
}

static void test_camera_view_projection(void)
{
    atom_camera_t cam;
    atom_camera_init(&cam, ATOM_CAMERA_MODE_FPS);
    atom_camera_set_perspective(&cam, 60.0f, 1.0f, 0.1f, 100.0f);
    atom_camera_set_position(&cam, atom_vec3_create(0, 0, 5));

    const atom_mat4_t *vp = atom_camera_get_view_projection(&cam);
    /* VP matrix should be non-zero */
    ASSERT_TRUE(vp->m[0] != 0.0f);
}

/* ========================================================================== *
 * FPS Camera Tests
 * ========================================================================== */

static void test_fps_camera_init(void)
{
    atom_camera_t cam;
    atom_fps_camera_init(&cam);
    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_FPS);
}

static void test_fps_bindings_default(void)
{
    atom_fps_camera_bindings_t bindings = atom_fps_camera_bindings_default();
    ASSERT_EQ(bindings.forward, ATOM_KEY_W);
    ASSERT_EQ(bindings.backward, ATOM_KEY_S);
    ASSERT_EQ(bindings.left, ATOM_KEY_A);
    ASSERT_EQ(bindings.right, ATOM_KEY_D);
    ASSERT_EQ(bindings.up, ATOM_KEY_SPACE);
    ASSERT_EQ(bindings.sprint, ATOM_KEY_LSHIFT);
}

static void test_fps_camera_move(void)
{
    atom_camera_t cam;
    atom_fps_camera_init(&cam);

    atom_vec3_t initial_pos = atom_camera_get_position(&cam);
    atom_fps_camera_move(&cam, atom_vec3_create(1, 0, 0), 5.0f);
    atom_vec3_t new_pos = atom_camera_get_position(&cam);

    ASSERT_FLOAT_NEAR(new_pos.x, initial_pos.x + 5.0f, 0.1f);
}

static void test_fps_camera_rotate(void)
{
    atom_camera_t cam;
    atom_fps_camera_init(&cam);

    /* Rotate 90 degrees yaw */
    atom_fps_camera_rotate_deg(&cam, 0.0f, 90.0f);
    atom_vec3_t fwd = atom_camera_get_forward(&cam);

    /* Forward should have changed significantly */
    ASSERT_TRUE(fabsf(fwd.x) > 0.5f || fabsf(fwd.z) < 0.5f);
}

static void test_fps_camera_move_local(void)
{
    atom_camera_t cam;
    atom_fps_camera_init(&cam);

    atom_vec3_t pos_before = atom_camera_get_position(&cam);
    /* Move forward in local space (-Z) */
    atom_fps_camera_move_local(&cam, atom_vec3_create(0, 0, -1), 3.0f);
    atom_vec3_t pos_after = atom_camera_get_position(&cam);

    /* Should have moved along the camera's forward direction */
    float dist = atom_vec3_distance(pos_before, pos_after);
    ASSERT_FLOAT_NEAR(dist, 3.0f, 0.1f);
}

/* ========================================================================== *
 * Orbital Camera Tests
 * ========================================================================== */

static void test_orbital_camera_init(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    ASSERT_EQ(atom_camera_get_mode(&cam), ATOM_CAMERA_MODE_ORBITAL);
    ASSERT_FLOAT_EQ(cam.orbit_distance, 10.0f);
    ASSERT_VEC3_EQ(cam.focal_point, atom_vec3_zero());
}

static void test_orbital_bindings_default(void)
{
    atom_orbital_camera_bindings_t bindings = atom_orbital_camera_bindings_default();
    ASSERT_EQ(bindings.orbit_button, ATOM_MOUSE_LEFT);
    ASSERT_EQ(bindings.pan_button, ATOM_MOUSE_MIDDLE);
    ASSERT_EQ(bindings.reset_key, ATOM_KEY_R);
    ASSERT_EQ(bindings.focus_key, ATOM_KEY_F);
}

static void test_orbital_camera_zoom(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    atom_orbital_camera_zoom(&cam, -3.0f);
    /* Distance should decrease (zoom in) */
    ASSERT_FLOAT_NEAR(cam.orbit_distance, 7.0f, 0.5f);
}

static void test_orbital_camera_orbit(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    atom_vec3_t pos_before = atom_camera_get_position(&cam);
    atom_orbital_camera_orbit_deg(&cam, 45.0f, 0.0f);
    atom_orbital_camera_update_position(&cam);
    atom_vec3_t pos_after = atom_camera_get_position(&cam);

    /* Position should change */
    float dist = atom_vec3_distance(pos_before, pos_after);
    ASSERT_TRUE(dist > 0.1f);

    /* But distance to focal point should remain the same */
    float dist_to_focal = atom_vec3_distance(pos_after, atom_vec3_zero());
    ASSERT_FLOAT_NEAR(dist_to_focal, 10.0f, 0.5f);
}

static void test_orbital_camera_pan(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    atom_orbital_camera_pan(&cam, atom_vec2_create(1.0f, 0.0f));

    /* Focal point should have moved */
    float dist = atom_vec3_distance(cam.focal_point, atom_vec3_zero());
    ASSERT_TRUE(dist > 0.01f);
}

static void test_orbital_camera_set_target(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    atom_vec3_t new_target = atom_vec3_create(5, 5, 5);
    atom_orbital_camera_set_target(&cam, new_target);
    ASSERT_VEC3_EQ(cam.focal_point, new_target);
}

static void test_orbital_camera_reset(void)
{
    atom_camera_t cam;
    atom_orbital_camera_init(&cam, atom_vec3_zero(), 10.0f);

    /* Move camera around */
    atom_orbital_camera_orbit_deg(&cam, 30.0f, 20.0f);
    atom_orbital_camera_zoom(&cam, -3.0f);

    /* Reset */
    atom_vec3_t new_focal = atom_vec3_create(1, 2, 3);
    atom_orbital_camera_reset(&cam, new_focal, 15.0f, 0.0f, 0.0f);

    ASSERT_FLOAT_NEAR(cam.orbit_distance, 15.0f, 0.1f);
    ASSERT_VEC3_EQ(cam.focal_point, new_focal);
}

/* ========================================================================== *
 * Input System Integration Tests
 * ========================================================================== */

static void test_input_system_lifecycle(void)
{
    atom_input_system_t sys;
    ASSERT_TRUE(atom_input_init(&sys));
    ASSERT_TRUE(sys.initialized);
    ASSERT_FALSE(atom_input_should_quit(&sys));

    atom_input_shutdown(&sys);
    ASSERT_FALSE(sys.initialized);
}

static void test_input_system_frame_cycle(void)
{
    atom_input_system_t sys;
    atom_input_init(&sys);

    atom_input_begin_frame(&sys, 1000);

    /* Push an event */
    atom_input_event_t event = {0};
    event.type = ATOM_EVENT_KEY_DOWN;
    event.data.key.key = ATOM_KEY_A;
    event.timestamp_ms = 1000;
    atom_event_queue_push(&sys.event_queue, &event);

    atom_input_process_events(&sys);
    atom_input_end_frame(&sys);

    /* Key should be registered */
    const atom_input_state_t *state = atom_input_get_state(&sys);
    ASSERT_TRUE(atom_key_is_down(state, ATOM_KEY_A));

    atom_input_shutdown(&sys);
}

static void test_input_system_quit(void)
{
    atom_input_system_t sys;
    atom_input_init(&sys);

    atom_input_begin_frame(&sys, 100);
    atom_input_event_t quit = {0};
    quit.type = ATOM_EVENT_WINDOW_CLOSE;
    atom_event_queue_push(&sys.event_queue, &quit);
    atom_input_process_events(&sys);

    ASSERT_TRUE(atom_input_should_quit(&sys));
    atom_input_shutdown(&sys);
}

static void test_input_system_mouse_capture(void)
{
    atom_input_system_t sys;
    atom_input_init(&sys);

    ASSERT_FALSE(atom_input_is_mouse_captured(&sys));
    atom_input_capture_mouse(&sys, true);
    ASSERT_TRUE(atom_input_is_mouse_captured(&sys));
    atom_input_capture_mouse(&sys, false);
    ASSERT_FALSE(atom_input_is_mouse_captured(&sys));

    atom_input_shutdown(&sys);
}

/* ========================================================================== *
 * Main
 * ========================================================================== */

int main(void)
{
    printf("=== camera & input_system tests ===\n\n");

    printf("[Base Camera]\n");
    RUN_TEST(test_camera_init_fps);
    RUN_TEST(test_camera_init_orbital);
    RUN_TEST(test_camera_set_position);
    RUN_TEST(test_camera_dirty_flags);
    RUN_TEST(test_camera_perspective);
    RUN_TEST(test_camera_set_fov);
    RUN_TEST(test_camera_set_aspect);
    RUN_TEST(test_camera_view_matrix);
    RUN_TEST(test_camera_direction_vectors);
    RUN_TEST(test_camera_set_euler);
    RUN_TEST(test_camera_look_at);
    RUN_TEST(test_camera_mode_switch);
    RUN_TEST(test_camera_view_projection);

    printf("\n[FPS Camera]\n");
    RUN_TEST(test_fps_camera_init);
    RUN_TEST(test_fps_bindings_default);
    RUN_TEST(test_fps_camera_move);
    RUN_TEST(test_fps_camera_rotate);
    RUN_TEST(test_fps_camera_move_local);

    printf("\n[Orbital Camera]\n");
    RUN_TEST(test_orbital_camera_init);
    RUN_TEST(test_orbital_bindings_default);
    RUN_TEST(test_orbital_camera_zoom);
    RUN_TEST(test_orbital_camera_orbit);
    RUN_TEST(test_orbital_camera_pan);
    RUN_TEST(test_orbital_camera_set_target);
    RUN_TEST(test_orbital_camera_reset);

    printf("\n[Input System]\n");
    RUN_TEST(test_input_system_lifecycle);
    RUN_TEST(test_input_system_frame_cycle);
    RUN_TEST(test_input_system_quit);
    RUN_TEST(test_input_system_mouse_capture);

    TEST_SUMMARY();
}
