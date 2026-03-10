/**
 * @file atom_math.h
 * @brief Math types and operations for ATOM game engine
 *
 * Provides Vec3, Quaternion, and Mat4 types with common operations.
 * Uses column-major matrices for OpenGL compatibility.
 */

#ifndef ATOM_MATH_H_
#define ATOM_MATH_H_

#include <math.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ============================================================================
     * Constants
     * ============================================================================ */

#define ATOM_PI         3.14159265358979323846f
#define ATOM_TWO_PI     6.28318530717958647692f
#define ATOM_HALF_PI    1.57079632679489661923f
#define ATOM_DEG_TO_RAD (ATOM_PI / 180.0f)
#define ATOM_RAD_TO_DEG (180.0f / ATOM_PI)
#define ATOM_EPSILON    1e-6f

    /* ============================================================================
     * Vec2 - 2D Vector
     * ============================================================================ */

    typedef struct atom_vec2
    {
        float x;
        float y;
    } atom_vec2_t;

    atom_vec2_t atom_vec2_create(float x, float y);
    atom_vec2_t atom_vec2_zero(void);
    atom_vec2_t atom_vec2_add(atom_vec2_t a, atom_vec2_t b);
    atom_vec2_t atom_vec2_sub(atom_vec2_t a, atom_vec2_t b);
    atom_vec2_t atom_vec2_scale(atom_vec2_t v, float s);
    float atom_vec2_dot(atom_vec2_t a, atom_vec2_t b);
    float atom_vec2_length(atom_vec2_t v);
    float atom_vec2_length_sq(atom_vec2_t v);
    atom_vec2_t atom_vec2_normalize(atom_vec2_t v);

    /* ============================================================================
     * Vec3 - 3D Vector
     * ============================================================================ */

    typedef struct atom_vec3
    {
        float x;
        float y;
        float z;
    } atom_vec3_t;

    /* Constructors */
    atom_vec3_t atom_vec3_create(float x, float y, float z);
    atom_vec3_t atom_vec3_zero(void);
    atom_vec3_t atom_vec3_one(void);
    atom_vec3_t atom_vec3_up(void);      /* (0, 1, 0) */
    atom_vec3_t atom_vec3_down(void);    /* (0, -1, 0) */
    atom_vec3_t atom_vec3_forward(void); /* (0, 0, -1) OpenGL convention */
    atom_vec3_t atom_vec3_back(void);    /* (0, 0, 1) */
    atom_vec3_t atom_vec3_right(void);   /* (1, 0, 0) */
    atom_vec3_t atom_vec3_left(void);    /* (-1, 0, 0) */

    /* Basic operations */
    atom_vec3_t atom_vec3_add(atom_vec3_t a, atom_vec3_t b);
    atom_vec3_t atom_vec3_sub(atom_vec3_t a, atom_vec3_t b);
    atom_vec3_t atom_vec3_mul(atom_vec3_t a, atom_vec3_t b); /* Component-wise */
    atom_vec3_t atom_vec3_div(atom_vec3_t a, atom_vec3_t b); /* Component-wise */
    atom_vec3_t atom_vec3_scale(atom_vec3_t v, float s);
    atom_vec3_t atom_vec3_negate(atom_vec3_t v);

    /* Products */
    float atom_vec3_dot(atom_vec3_t a, atom_vec3_t b);
    atom_vec3_t atom_vec3_cross(atom_vec3_t a, atom_vec3_t b);

    /* Length */
    float atom_vec3_length(atom_vec3_t v);
    float atom_vec3_length_sq(atom_vec3_t v);
    float atom_vec3_distance(atom_vec3_t a, atom_vec3_t b);
    float atom_vec3_distance_sq(atom_vec3_t a, atom_vec3_t b);

    /* Normalization */
    atom_vec3_t atom_vec3_normalize(atom_vec3_t v);
    bool atom_vec3_is_normalized(atom_vec3_t v);

    /* Interpolation */
    atom_vec3_t atom_vec3_lerp(atom_vec3_t a, atom_vec3_t b, float t);

    /* Comparison */
    bool atom_vec3_equals(atom_vec3_t a, atom_vec3_t b, float epsilon);

    /* Utility */
    atom_vec3_t atom_vec3_reflect(atom_vec3_t v, atom_vec3_t normal);
    atom_vec3_t atom_vec3_project(atom_vec3_t v, atom_vec3_t onto);

    /* ============================================================================
     * Vec4 - 4D Vector / Homogeneous coordinates
     * ============================================================================ */

    typedef struct atom_vec4
    {
        float x;
        float y;
        float z;
        float w;
    } atom_vec4_t;

    atom_vec4_t atom_vec4_create(float x, float y, float z, float w);
    atom_vec4_t atom_vec4_from_vec3(atom_vec3_t v, float w);
    atom_vec3_t atom_vec4_to_vec3(atom_vec4_t v);
    atom_vec4_t atom_vec4_zero(void);
    atom_vec4_t atom_vec4_add(atom_vec4_t a, atom_vec4_t b);
    atom_vec4_t atom_vec4_sub(atom_vec4_t a, atom_vec4_t b);
    atom_vec4_t atom_vec4_scale(atom_vec4_t v, float s);
    float atom_vec4_dot(atom_vec4_t a, atom_vec4_t b);
    float atom_vec4_length(atom_vec4_t v);
    atom_vec4_t atom_vec4_normalize(atom_vec4_t v);

    /* ============================================================================
     * Quaternion - Rotation representation
     * ============================================================================ */

    typedef struct atom_quat
    {
        float w; /* Scalar component (real) */
        float x; /* Vector components (imaginary) */
        float y;
        float z;
    } atom_quat_t;

    /* Constructors */
    atom_quat_t atom_quat_identity(void);
    atom_quat_t atom_quat_create(float w, float x, float y, float z);
    atom_quat_t atom_quat_from_axis_angle(atom_vec3_t axis, float angle_rad);
    atom_quat_t atom_quat_from_euler(float pitch, float yaw, float roll); /* Radians, ZYX order */
    atom_quat_t atom_quat_from_euler_deg(float pitch_deg, float yaw_deg, float roll_deg);

    /* Operations */
    atom_quat_t atom_quat_multiply(atom_quat_t a, atom_quat_t b);
    atom_quat_t atom_quat_conjugate(atom_quat_t q);
    atom_quat_t atom_quat_inverse(atom_quat_t q);
    atom_quat_t atom_quat_normalize(atom_quat_t q);
    float atom_quat_length(atom_quat_t q);
    float atom_quat_length_sq(atom_quat_t q);
    float atom_quat_dot(atom_quat_t a, atom_quat_t b);

    /* Rotation */
    atom_vec3_t atom_quat_rotate_vec3(atom_quat_t q, atom_vec3_t v);

    /* Interpolation */
    atom_quat_t atom_quat_lerp(atom_quat_t a, atom_quat_t b, float t);
    atom_quat_t atom_quat_slerp(atom_quat_t a, atom_quat_t b, float t);

    /* Extraction */
    void atom_quat_to_euler(atom_quat_t q, float *pitch, float *yaw, float *roll);
    void atom_quat_to_axis_angle(atom_quat_t q, atom_vec3_t *axis, float *angle);
    atom_vec3_t atom_quat_get_forward(atom_quat_t q);
    atom_vec3_t atom_quat_get_right(atom_quat_t q);
    atom_vec3_t atom_quat_get_up(atom_quat_t q);

    /* Comparison */
    bool atom_quat_equals(atom_quat_t a, atom_quat_t b, float epsilon);

    /* ============================================================================
     * Mat4 - 4x4 Matrix (column-major for OpenGL compatibility)
     *
     * Memory layout:
     *   m[0]  m[4]  m[8]   m[12]     | col0.x  col1.x  col2.x  col3.x |
     *   m[1]  m[5]  m[9]   m[13]  =  | col0.y  col1.y  col2.y  col3.y |
     *   m[2]  m[6]  m[10]  m[14]     | col0.z  col1.z  col2.z  col3.z |
     *   m[3]  m[7]  m[11]  m[15]     | col0.w  col1.w  col2.w  col3.w |
     *
     * Access: m[col * 4 + row] or m[col][row]
     * ============================================================================ */

    typedef struct atom_mat4
    {
        float m[16];
    } atom_mat4_t;

    /* Constructors */
    atom_mat4_t atom_mat4_identity(void);
    atom_mat4_t atom_mat4_zero(void);
    atom_mat4_t atom_mat4_from_quat(atom_quat_t q);
    atom_mat4_t atom_mat4_from_cols(atom_vec4_t c0, atom_vec4_t c1, atom_vec4_t c2, atom_vec4_t c3);

    /* Transform construction */
    atom_mat4_t atom_mat4_translate(atom_vec3_t translation);
    atom_mat4_t atom_mat4_scale(atom_vec3_t scale);
    atom_mat4_t atom_mat4_scale_uniform(float scale);
    atom_mat4_t atom_mat4_rotate_x(float angle_rad);
    atom_mat4_t atom_mat4_rotate_y(float angle_rad);
    atom_mat4_t atom_mat4_rotate_z(float angle_rad);
    atom_mat4_t atom_mat4_rotate_axis(atom_vec3_t axis, float angle_rad);
    atom_mat4_t atom_mat4_rotate_quat(atom_quat_t q);

    /* View matrices */
    atom_mat4_t atom_mat4_look_at(atom_vec3_t eye, atom_vec3_t target, atom_vec3_t up);

    /* Projection matrices */
    atom_mat4_t atom_mat4_perspective(float fov_rad, float aspect, float near_plane,
                                      float far_plane);
    atom_mat4_t atom_mat4_perspective_deg(float fov_deg, float aspect, float near_plane,
                                          float far_plane);
    atom_mat4_t atom_mat4_ortho(float left, float right, float bottom, float top, float near_plane,
                                float far_plane);

    /* Operations */
    atom_mat4_t atom_mat4_multiply(atom_mat4_t a, atom_mat4_t b);
    atom_vec4_t atom_mat4_multiply_vec4(atom_mat4_t m, atom_vec4_t v);
    atom_vec3_t atom_mat4_transform_point(atom_mat4_t m, atom_vec3_t p);
    atom_vec3_t atom_mat4_transform_direction(atom_mat4_t m, atom_vec3_t d);
    atom_mat4_t atom_mat4_transpose(atom_mat4_t m);
    atom_mat4_t atom_mat4_inverse(atom_mat4_t m);
    float atom_mat4_determinant(atom_mat4_t m);

    /* Extraction */
    atom_vec3_t atom_mat4_get_translation(atom_mat4_t m);
    atom_vec3_t atom_mat4_get_scale(atom_mat4_t m);
    atom_quat_t atom_mat4_get_rotation(atom_mat4_t m);

    /* Element access */
    float atom_mat4_get(atom_mat4_t m, int row, int col);
    void atom_mat4_set(atom_mat4_t *m, int row, int col, float value);

    /* ============================================================================
     * Utility Functions
     * ============================================================================ */

    float atom_clampf(float value, float min_val, float max_val);
    float atom_lerpf(float a, float b, float t);
    float atom_minf(float a, float b);
    float atom_maxf(float a, float b);
    float atom_absf(float x);
    float atom_signf(float x);
    float atom_wrap_angle(float angle_rad); /* Wrap to [-PI, PI] */
    bool atom_approx_equal(float a, float b, float epsilon);

#ifdef __cplusplus
}
#endif

#endif /* !ATOM_MATH_H_ */
