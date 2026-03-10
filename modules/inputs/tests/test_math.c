/**
 * @file test_math.c
 * @brief Unit tests for atom_math: Vec2, Vec3, Vec4, Quaternion, Mat4, utilities
 */

#include "test_helpers.h"
#include <inputs/atom_math.h>

/* ========================================================================== *
 * Vec2 Tests
 * ========================================================================== */

static void test_vec2_create(void)
{
    atom_vec2_t v = atom_vec2_create(3.0f, 4.0f);
    ASSERT_FLOAT_EQ(v.x, 3.0f);
    ASSERT_FLOAT_EQ(v.y, 4.0f);
}

static void test_vec2_zero(void)
{
    atom_vec2_t v = atom_vec2_zero();
    ASSERT_FLOAT_EQ(v.x, 0.0f);
    ASSERT_FLOAT_EQ(v.y, 0.0f);
}

static void test_vec2_add(void)
{
    atom_vec2_t a = atom_vec2_create(1.0f, 2.0f);
    atom_vec2_t b = atom_vec2_create(3.0f, 4.0f);
    atom_vec2_t c = atom_vec2_add(a, b);
    ASSERT_FLOAT_EQ(c.x, 4.0f);
    ASSERT_FLOAT_EQ(c.y, 6.0f);
}

static void test_vec2_sub(void)
{
    atom_vec2_t a = atom_vec2_create(5.0f, 7.0f);
    atom_vec2_t b = atom_vec2_create(2.0f, 3.0f);
    atom_vec2_t c = atom_vec2_sub(a, b);
    ASSERT_FLOAT_EQ(c.x, 3.0f);
    ASSERT_FLOAT_EQ(c.y, 4.0f);
}

static void test_vec2_scale(void)
{
    atom_vec2_t v = atom_vec2_create(2.0f, 3.0f);
    atom_vec2_t r = atom_vec2_scale(v, 2.0f);
    ASSERT_FLOAT_EQ(r.x, 4.0f);
    ASSERT_FLOAT_EQ(r.y, 6.0f);
}

static void test_vec2_dot(void)
{
    atom_vec2_t a = atom_vec2_create(1.0f, 0.0f);
    atom_vec2_t b = atom_vec2_create(0.0f, 1.0f);
    ASSERT_FLOAT_EQ(atom_vec2_dot(a, b), 0.0f);

    atom_vec2_t c = atom_vec2_create(2.0f, 3.0f);
    atom_vec2_t d = atom_vec2_create(4.0f, 5.0f);
    ASSERT_FLOAT_EQ(atom_vec2_dot(c, d), 23.0f);
}

static void test_vec2_length(void)
{
    atom_vec2_t v = atom_vec2_create(3.0f, 4.0f);
    ASSERT_FLOAT_EQ(atom_vec2_length(v), 5.0f);
    ASSERT_FLOAT_EQ(atom_vec2_length_sq(v), 25.0f);
}

static void test_vec2_normalize(void)
{
    atom_vec2_t v = atom_vec2_create(3.0f, 4.0f);
    atom_vec2_t n = atom_vec2_normalize(v);
    ASSERT_FLOAT_EQ(atom_vec2_length(n), 1.0f);
    ASSERT_FLOAT_EQ(n.x, 3.0f / 5.0f);
    ASSERT_FLOAT_EQ(n.y, 4.0f / 5.0f);
}

/* ========================================================================== *
 * Vec3 Tests
 * ========================================================================== */

static void test_vec3_create(void)
{
    atom_vec3_t v = atom_vec3_create(1.0f, 2.0f, 3.0f);
    ASSERT_FLOAT_EQ(v.x, 1.0f);
    ASSERT_FLOAT_EQ(v.y, 2.0f);
    ASSERT_FLOAT_EQ(v.z, 3.0f);
}

static void test_vec3_constructors(void)
{
    ASSERT_VEC3_EQ(atom_vec3_zero(), atom_vec3_create(0, 0, 0));
    ASSERT_VEC3_EQ(atom_vec3_one(), atom_vec3_create(1, 1, 1));
    ASSERT_VEC3_EQ(atom_vec3_up(), atom_vec3_create(0, 1, 0));
    ASSERT_VEC3_EQ(atom_vec3_down(), atom_vec3_create(0, -1, 0));
    ASSERT_VEC3_EQ(atom_vec3_forward(), atom_vec3_create(0, 0, -1));
    ASSERT_VEC3_EQ(atom_vec3_back(), atom_vec3_create(0, 0, 1));
    ASSERT_VEC3_EQ(atom_vec3_right(), atom_vec3_create(1, 0, 0));
    ASSERT_VEC3_EQ(atom_vec3_left(), atom_vec3_create(-1, 0, 0));
}

static void test_vec3_add_sub(void)
{
    atom_vec3_t a = atom_vec3_create(1, 2, 3);
    atom_vec3_t b = atom_vec3_create(4, 5, 6);
    ASSERT_VEC3_EQ(atom_vec3_add(a, b), atom_vec3_create(5, 7, 9));
    ASSERT_VEC3_EQ(atom_vec3_sub(a, b), atom_vec3_create(-3, -3, -3));
}

static void test_vec3_scale(void)
{
    atom_vec3_t v = atom_vec3_create(1, 2, 3);
    ASSERT_VEC3_EQ(atom_vec3_scale(v, 2.0f), atom_vec3_create(2, 4, 6));
    ASSERT_VEC3_EQ(atom_vec3_scale(v, -1.0f), atom_vec3_negate(v));
}

static void test_vec3_dot(void)
{
    atom_vec3_t a = atom_vec3_create(1, 0, 0);
    atom_vec3_t b = atom_vec3_create(0, 1, 0);
    ASSERT_FLOAT_EQ(atom_vec3_dot(a, b), 0.0f);

    atom_vec3_t c = atom_vec3_create(2, 3, 4);
    atom_vec3_t d = atom_vec3_create(5, 6, 7);
    ASSERT_FLOAT_EQ(atom_vec3_dot(c, d), 56.0f);
}

static void test_vec3_cross(void)
{
    atom_vec3_t x = atom_vec3_create(1, 0, 0);
    atom_vec3_t y = atom_vec3_create(0, 1, 0);
    atom_vec3_t z = atom_vec3_cross(x, y);
    ASSERT_VEC3_EQ(z, atom_vec3_create(0, 0, 1));

    /* Anti-commutativity */
    atom_vec3_t z_neg = atom_vec3_cross(y, x);
    ASSERT_VEC3_EQ(z_neg, atom_vec3_create(0, 0, -1));
}

static void test_vec3_length(void)
{
    atom_vec3_t v = atom_vec3_create(1, 2, 2);
    ASSERT_FLOAT_EQ(atom_vec3_length(v), 3.0f);
    ASSERT_FLOAT_EQ(atom_vec3_length_sq(v), 9.0f);
}

static void test_vec3_normalize(void)
{
    atom_vec3_t v = atom_vec3_create(3, 0, 4);
    atom_vec3_t n = atom_vec3_normalize(v);
    ASSERT_FLOAT_EQ(atom_vec3_length(n), 1.0f);
    ASSERT_FLOAT_EQ(n.x, 0.6f);
    ASSERT_FLOAT_EQ(n.z, 0.8f);

    ASSERT_TRUE(atom_vec3_is_normalized(n));
    ASSERT_FALSE(atom_vec3_is_normalized(v));
}

static void test_vec3_distance(void)
{
    atom_vec3_t a = atom_vec3_create(1, 0, 0);
    atom_vec3_t b = atom_vec3_create(4, 0, 0);
    ASSERT_FLOAT_EQ(atom_vec3_distance(a, b), 3.0f);
    ASSERT_FLOAT_EQ(atom_vec3_distance_sq(a, b), 9.0f);
}

static void test_vec3_lerp(void)
{
    atom_vec3_t a = atom_vec3_create(0, 0, 0);
    atom_vec3_t b = atom_vec3_create(10, 20, 30);
    ASSERT_VEC3_EQ(atom_vec3_lerp(a, b, 0.0f), a);
    ASSERT_VEC3_EQ(atom_vec3_lerp(a, b, 1.0f), b);
    ASSERT_VEC3_EQ(atom_vec3_lerp(a, b, 0.5f), atom_vec3_create(5, 10, 15));
}

static void test_vec3_reflect(void)
{
    atom_vec3_t v = atom_vec3_create(1, -1, 0);
    atom_vec3_t n = atom_vec3_create(0, 1, 0);
    atom_vec3_t r = atom_vec3_reflect(v, n);
    ASSERT_VEC3_EQ(r, atom_vec3_create(1, 1, 0));
}

static void test_vec3_project(void)
{
    atom_vec3_t v = atom_vec3_create(3, 4, 0);
    atom_vec3_t onto = atom_vec3_create(1, 0, 0);
    atom_vec3_t p = atom_vec3_project(v, onto);
    ASSERT_VEC3_EQ(p, atom_vec3_create(3, 0, 0));
}

static void test_vec3_equals(void)
{
    atom_vec3_t a = atom_vec3_create(1.0f, 2.0f, 3.0f);
    atom_vec3_t b = atom_vec3_create(1.0f, 2.0f, 3.0f);
    atom_vec3_t c = atom_vec3_create(1.1f, 2.0f, 3.0f);
    ASSERT_TRUE(atom_vec3_equals(a, b, ATOM_EPSILON));
    ASSERT_FALSE(atom_vec3_equals(a, c, ATOM_EPSILON));
}

/* ========================================================================== *
 * Vec4 Tests
 * ========================================================================== */

static void test_vec4_create(void)
{
    atom_vec4_t v = atom_vec4_create(1, 2, 3, 4);
    ASSERT_FLOAT_EQ(v.x, 1.0f);
    ASSERT_FLOAT_EQ(v.y, 2.0f);
    ASSERT_FLOAT_EQ(v.z, 3.0f);
    ASSERT_FLOAT_EQ(v.w, 4.0f);
}

static void test_vec4_conversions(void)
{
    atom_vec3_t v3 = atom_vec3_create(1, 2, 3);
    atom_vec4_t v4 = atom_vec4_from_vec3(v3, 1.0f);
    ASSERT_FLOAT_EQ(v4.x, 1.0f);
    ASSERT_FLOAT_EQ(v4.w, 1.0f);

    atom_vec3_t back = atom_vec4_to_vec3(v4);
    ASSERT_VEC3_EQ(back, v3);
}

static void test_vec4_ops(void)
{
    atom_vec4_t a = atom_vec4_create(1, 2, 3, 4);
    atom_vec4_t b = atom_vec4_create(5, 6, 7, 8);
    atom_vec4_t c = atom_vec4_add(a, b);
    ASSERT_FLOAT_EQ(c.x, 6.0f);
    ASSERT_FLOAT_EQ(c.w, 12.0f);

    atom_vec4_t d = atom_vec4_scale(a, 2.0f);
    ASSERT_FLOAT_EQ(d.x, 2.0f);
    ASSERT_FLOAT_EQ(d.w, 8.0f);

    ASSERT_FLOAT_EQ(atom_vec4_dot(a, b), 70.0f);
}

/* ========================================================================== *
 * Quaternion Tests
 * ========================================================================== */

static void test_quat_identity(void)
{
    atom_quat_t q = atom_quat_identity();
    ASSERT_FLOAT_EQ(q.w, 1.0f);
    ASSERT_FLOAT_EQ(q.x, 0.0f);
    ASSERT_FLOAT_EQ(q.y, 0.0f);
    ASSERT_FLOAT_EQ(q.z, 0.0f);
}

static void test_quat_from_axis_angle(void)
{
    /* 90 degrees around Y axis */
    atom_quat_t q = atom_quat_from_axis_angle(atom_vec3_up(), ATOM_HALF_PI);
    ASSERT_FLOAT_NEAR(atom_quat_length(q), 1.0f, 1e-5f);

    /* Rotating (1,0,0) 90 deg around Y should give (0,0,-1) */
    atom_vec3_t v = atom_vec3_create(1, 0, 0);
    atom_vec3_t r = atom_quat_rotate_vec3(q, v);
    ASSERT_VEC3_NEAR(r, atom_vec3_create(0, 0, -1), 1e-4f);
}

static void test_quat_from_euler(void)
{
    /* Identity: no rotation */
    atom_quat_t q = atom_quat_from_euler(0, 0, 0);
    ASSERT_QUAT_NEAR(q, atom_quat_identity(), 1e-5f);

    /* Roundtrip: from_euler -> to_euler should preserve angles */
    float p = 0.2f, y = 0.5f, r = 0.0f;
    atom_quat_t q2 = atom_quat_from_euler(p, y, r);
    float p2, y2, r2;
    atom_quat_to_euler(q2, &p2, &y2, &r2);
    ASSERT_FLOAT_NEAR(p2, p, 1e-3f);
    ASSERT_FLOAT_NEAR(y2, y, 1e-3f);
}

static void test_quat_multiply(void)
{
    /* Two 90-degree rotations around Y = 180 degrees */
    atom_quat_t q90 = atom_quat_from_axis_angle(atom_vec3_up(), ATOM_HALF_PI);
    atom_quat_t q180 = atom_quat_multiply(q90, q90);

    atom_vec3_t v = atom_vec3_create(1, 0, 0);
    atom_vec3_t r = atom_quat_rotate_vec3(q180, v);
    ASSERT_VEC3_NEAR(r, atom_vec3_create(-1, 0, 0), 1e-4f);
}

static void test_quat_conjugate_inverse(void)
{
    atom_quat_t q = atom_quat_from_axis_angle(atom_vec3_up(), 0.5f);
    atom_quat_t conj = atom_quat_conjugate(q);
    ASSERT_FLOAT_EQ(conj.w, q.w);
    ASSERT_FLOAT_EQ(conj.x, -q.x);
    ASSERT_FLOAT_EQ(conj.y, -q.y);
    ASSERT_FLOAT_EQ(conj.z, -q.z);

    /* q * q^-1 = identity */
    atom_quat_t inv = atom_quat_inverse(q);
    atom_quat_t id = atom_quat_multiply(q, inv);
    ASSERT_QUAT_NEAR(id, atom_quat_identity(), 1e-4f);
}

static void test_quat_slerp(void)
{
    atom_quat_t a = atom_quat_identity();
    atom_quat_t b = atom_quat_from_axis_angle(atom_vec3_up(), ATOM_PI);

    /* t=0 -> a */
    atom_quat_t r0 = atom_quat_slerp(a, b, 0.0f);
    ASSERT_QUAT_NEAR(r0, a, 1e-4f);

    /* t=1 -> b */
    atom_quat_t r1 = atom_quat_slerp(a, b, 1.0f);
    ASSERT_QUAT_NEAR(r1, b, 1e-4f);

    /* t=0.5 -> halfway rotation: resulting quaternion should be unit length */
    atom_quat_t r05 = atom_quat_slerp(a, b, 0.5f);
    ASSERT_FLOAT_NEAR(atom_quat_length(r05), 1.0f, 1e-4f);
}

static void test_quat_to_euler_roundtrip(void)
{
    float pitch = 0.3f, yaw = 0.7f, roll = 0.1f;
    atom_quat_t q = atom_quat_from_euler(pitch, yaw, roll);

    float p2, y2, r2;
    atom_quat_to_euler(q, &p2, &y2, &r2);
    ASSERT_FLOAT_NEAR(p2, pitch, 1e-3f);
    ASSERT_FLOAT_NEAR(y2, yaw, 1e-3f);
    ASSERT_FLOAT_NEAR(r2, roll, 1e-3f);
}

static void test_quat_direction_vectors(void)
{
    atom_quat_t q = atom_quat_identity();
    atom_vec3_t fwd = atom_quat_get_forward(q);
    atom_vec3_t rgt = atom_quat_get_right(q);
    atom_vec3_t up = atom_quat_get_up(q);

    /* For identity quaternion: forward = (0,0,-1), right = (1,0,0), up = (0,1,0) */
    ASSERT_VEC3_NEAR(fwd, atom_vec3_create(0, 0, -1), 1e-4f);
    ASSERT_VEC3_NEAR(rgt, atom_vec3_create(1, 0, 0), 1e-4f);
    ASSERT_VEC3_NEAR(up, atom_vec3_create(0, 1, 0), 1e-4f);
}

/* ========================================================================== *
 * Mat4 Tests
 * ========================================================================== */

static void test_mat4_identity(void)
{
    atom_mat4_t id = atom_mat4_identity();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float expected = (i == j) ? 1.0f : 0.0f;
            ASSERT_FLOAT_EQ(atom_mat4_get(id, i, j), expected);
        }
    }
}

static void test_mat4_multiply_identity(void)
{
    atom_mat4_t id = atom_mat4_identity();
    atom_mat4_t tr = atom_mat4_translate(atom_vec3_create(1, 2, 3));
    atom_mat4_t result = atom_mat4_multiply(id, tr);
    for (int i = 0; i < 16; i++)
    {
        ASSERT_FLOAT_EQ(result.m[i], tr.m[i]);
    }
}

static void test_mat4_translate(void)
{
    atom_mat4_t tr = atom_mat4_translate(atom_vec3_create(10, 20, 30));
    atom_vec3_t p = atom_mat4_transform_point(tr, atom_vec3_zero());
    ASSERT_VEC3_EQ(p, atom_vec3_create(10, 20, 30));
}

static void test_mat4_scale(void)
{
    atom_mat4_t sc = atom_mat4_scale(atom_vec3_create(2, 3, 4));
    atom_vec3_t p = atom_mat4_transform_point(sc, atom_vec3_create(1, 1, 1));
    ASSERT_VEC3_EQ(p, atom_vec3_create(2, 3, 4));
}

static void test_mat4_rotate(void)
{
    /* 90 degrees around Y: (1,0,0) -> (0,0,-1) */
    atom_mat4_t rot = atom_mat4_rotate_y(ATOM_HALF_PI);
    atom_vec3_t p = atom_mat4_transform_point(rot, atom_vec3_create(1, 0, 0));
    ASSERT_VEC3_NEAR(p, atom_vec3_create(0, 0, -1), 1e-4f);
}

static void test_mat4_from_quat(void)
{
    atom_quat_t q = atom_quat_from_axis_angle(atom_vec3_up(), ATOM_HALF_PI);
    atom_mat4_t m = atom_mat4_from_quat(q);
    atom_vec3_t p = atom_mat4_transform_point(m, atom_vec3_create(1, 0, 0));
    ASSERT_VEC3_NEAR(p, atom_vec3_create(0, 0, -1), 1e-4f);
}

static void test_mat4_transpose(void)
{
    atom_mat4_t m = atom_mat4_identity();
    atom_mat4_set(&m, 0, 1, 5.0f);
    atom_mat4_t t = atom_mat4_transpose(m);
    ASSERT_FLOAT_EQ(atom_mat4_get(t, 1, 0), 5.0f);
    ASSERT_FLOAT_EQ(atom_mat4_get(t, 0, 1), 0.0f);
}

static void test_mat4_inverse(void)
{
    atom_mat4_t tr = atom_mat4_translate(atom_vec3_create(5, 10, 15));
    atom_mat4_t inv = atom_mat4_inverse(tr);
    atom_mat4_t id = atom_mat4_multiply(tr, inv);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float expected = (i == j) ? 1.0f : 0.0f;
            ASSERT_FLOAT_NEAR(atom_mat4_get(id, i, j), expected, 1e-4f);
        }
    }
}

static void test_mat4_determinant(void)
{
    atom_mat4_t id = atom_mat4_identity();
    ASSERT_FLOAT_EQ(atom_mat4_determinant(id), 1.0f);

    atom_mat4_t sc = atom_mat4_scale(atom_vec3_create(2, 3, 4));
    ASSERT_FLOAT_EQ(atom_mat4_determinant(sc), 24.0f);
}

static void test_mat4_perspective(void)
{
    atom_mat4_t p = atom_mat4_perspective_deg(90.0f, 1.0f, 0.1f, 100.0f);
    /* Perspective matrix should be non-zero */
    ASSERT_TRUE(atom_mat4_get(p, 0, 0) != 0.0f);
    ASSERT_TRUE(atom_mat4_get(p, 1, 1) != 0.0f);
    ASSERT_TRUE(atom_mat4_get(p, 2, 2) != 0.0f);
    ASSERT_FLOAT_EQ(atom_mat4_get(p, 3, 2), -1.0f);
}

static void test_mat4_look_at(void)
{
    atom_vec3_t eye = atom_vec3_create(0, 0, 5);
    atom_vec3_t target = atom_vec3_zero();
    atom_vec3_t up = atom_vec3_up();
    atom_mat4_t view = atom_mat4_look_at(eye, target, up);

    /* Eye transformed should be at origin */
    atom_vec3_t origin = atom_mat4_transform_point(view, eye);
    ASSERT_VEC3_NEAR(origin, atom_vec3_zero(), 1e-4f);
}

static void test_mat4_extract(void)
{
    atom_vec3_t pos = atom_vec3_create(5, 10, 15);
    atom_mat4_t tr = atom_mat4_translate(pos);
    atom_vec3_t extracted = atom_mat4_get_translation(tr);
    ASSERT_VEC3_EQ(extracted, pos);

    atom_vec3_t sc = atom_vec3_create(2, 3, 4);
    atom_mat4_t scm = atom_mat4_scale(sc);
    atom_vec3_t extracted_sc = atom_mat4_get_scale(scm);
    ASSERT_VEC3_EQ(extracted_sc, sc);
}

static void test_mat4_transform_direction(void)
{
    /* Direction should NOT be affected by translation */
    atom_mat4_t tr = atom_mat4_translate(atom_vec3_create(100, 200, 300));
    atom_vec3_t d = atom_mat4_transform_direction(tr, atom_vec3_create(1, 0, 0));
    ASSERT_VEC3_EQ(d, atom_vec3_create(1, 0, 0));
}

/* ========================================================================== *
 * Utility Tests
 * ========================================================================== */

static void test_clamp(void)
{
    ASSERT_FLOAT_EQ(atom_clampf(5.0f, 0.0f, 10.0f), 5.0f);
    ASSERT_FLOAT_EQ(atom_clampf(-1.0f, 0.0f, 10.0f), 0.0f);
    ASSERT_FLOAT_EQ(atom_clampf(15.0f, 0.0f, 10.0f), 10.0f);
}

static void test_lerp(void)
{
    ASSERT_FLOAT_EQ(atom_lerpf(0.0f, 10.0f, 0.0f), 0.0f);
    ASSERT_FLOAT_EQ(atom_lerpf(0.0f, 10.0f, 1.0f), 10.0f);
    ASSERT_FLOAT_EQ(atom_lerpf(0.0f, 10.0f, 0.5f), 5.0f);
}

static void test_min_max(void)
{
    ASSERT_FLOAT_EQ(atom_minf(3.0f, 7.0f), 3.0f);
    ASSERT_FLOAT_EQ(atom_maxf(3.0f, 7.0f), 7.0f);
}

static void test_sign(void)
{
    ASSERT_FLOAT_EQ(atom_signf(5.0f), 1.0f);
    ASSERT_FLOAT_EQ(atom_signf(-5.0f), -1.0f);
    ASSERT_FLOAT_EQ(atom_signf(0.0f), 0.0f);
}

static void test_wrap_angle(void)
{
    float wrapped = atom_wrap_angle(ATOM_PI + 0.5f);
    ASSERT_TRUE(wrapped >= -ATOM_PI && wrapped <= ATOM_PI);

    ASSERT_FLOAT_NEAR(atom_wrap_angle(0.0f), 0.0f, 1e-5f);
}

static void test_approx_equal(void)
{
    ASSERT_TRUE(atom_approx_equal(1.0f, 1.0f + 1e-7f, ATOM_EPSILON));
    ASSERT_FALSE(atom_approx_equal(1.0f, 2.0f, ATOM_EPSILON));
}

/* ========================================================================== *
 * Main
 * ========================================================================== */

int main(void)
{
    printf("=== atom_math tests ===\n\n");

    /* Vec2 */
    printf("[Vec2]\n");
    RUN_TEST(test_vec2_create);
    RUN_TEST(test_vec2_zero);
    RUN_TEST(test_vec2_add);
    RUN_TEST(test_vec2_sub);
    RUN_TEST(test_vec2_scale);
    RUN_TEST(test_vec2_dot);
    RUN_TEST(test_vec2_length);
    RUN_TEST(test_vec2_normalize);

    /* Vec3 */
    printf("\n[Vec3]\n");
    RUN_TEST(test_vec3_create);
    RUN_TEST(test_vec3_constructors);
    RUN_TEST(test_vec3_add_sub);
    RUN_TEST(test_vec3_scale);
    RUN_TEST(test_vec3_dot);
    RUN_TEST(test_vec3_cross);
    RUN_TEST(test_vec3_length);
    RUN_TEST(test_vec3_normalize);
    RUN_TEST(test_vec3_distance);
    RUN_TEST(test_vec3_lerp);
    RUN_TEST(test_vec3_reflect);
    RUN_TEST(test_vec3_project);
    RUN_TEST(test_vec3_equals);

    /* Vec4 */
    printf("\n[Vec4]\n");
    RUN_TEST(test_vec4_create);
    RUN_TEST(test_vec4_conversions);
    RUN_TEST(test_vec4_ops);

    /* Quaternion */
    printf("\n[Quaternion]\n");
    RUN_TEST(test_quat_identity);
    RUN_TEST(test_quat_from_axis_angle);
    RUN_TEST(test_quat_from_euler);
    RUN_TEST(test_quat_multiply);
    RUN_TEST(test_quat_conjugate_inverse);
    RUN_TEST(test_quat_slerp);
    RUN_TEST(test_quat_to_euler_roundtrip);
    RUN_TEST(test_quat_direction_vectors);

    /* Mat4 */
    printf("\n[Mat4]\n");
    RUN_TEST(test_mat4_identity);
    RUN_TEST(test_mat4_multiply_identity);
    RUN_TEST(test_mat4_translate);
    RUN_TEST(test_mat4_scale);
    RUN_TEST(test_mat4_rotate);
    RUN_TEST(test_mat4_from_quat);
    RUN_TEST(test_mat4_transpose);
    RUN_TEST(test_mat4_inverse);
    RUN_TEST(test_mat4_determinant);
    RUN_TEST(test_mat4_perspective);
    RUN_TEST(test_mat4_look_at);
    RUN_TEST(test_mat4_extract);
    RUN_TEST(test_mat4_transform_direction);

    /* Utilities */
    printf("\n[Utilities]\n");
    RUN_TEST(test_clamp);
    RUN_TEST(test_lerp);
    RUN_TEST(test_min_max);
    RUN_TEST(test_sign);
    RUN_TEST(test_wrap_angle);
    RUN_TEST(test_approx_equal);

    TEST_SUMMARY();
}
