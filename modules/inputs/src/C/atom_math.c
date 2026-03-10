/**
 * @file atom_math.c
 * @brief Implementation of math types and operations
 */

#include "inputs/atom_math.h"

#include <string.h>

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

float atom_clampf(float value, float min_val, float max_val)
{
    if (value < min_val)
        return min_val;
    if (value > max_val)
        return max_val;
    return value;
}

float atom_lerpf(float a, float b, float t)
{
    return a + (b - a) * t;
}

float atom_minf(float a, float b)
{
    return a < b ? a : b;
}

float atom_maxf(float a, float b)
{
    return a > b ? a : b;
}

float atom_absf(float x)
{
    return x < 0.0f ? -x : x;
}

float atom_signf(float x)
{
    if (x > 0.0f)
        return 1.0f;
    if (x < 0.0f)
        return -1.0f;
    return 0.0f;
}

float atom_wrap_angle(float angle_rad)
{
    while (angle_rad > ATOM_PI)
        angle_rad -= ATOM_TWO_PI;
    while (angle_rad < -ATOM_PI)
        angle_rad += ATOM_TWO_PI;
    return angle_rad;
}

bool atom_approx_equal(float a, float b, float epsilon)
{
    return atom_absf(a - b) <= epsilon;
}

/* ============================================================================
 * Vec2 Implementation
 * ============================================================================ */

atom_vec2_t atom_vec2_create(float x, float y)
{
    atom_vec2_t v = {x, y};
    return v;
}

atom_vec2_t atom_vec2_zero(void)
{
    atom_vec2_t v = {0.0f, 0.0f};
    return v;
}

atom_vec2_t atom_vec2_add(atom_vec2_t a, atom_vec2_t b)
{
    atom_vec2_t v = {a.x + b.x, a.y + b.y};
    return v;
}

atom_vec2_t atom_vec2_sub(atom_vec2_t a, atom_vec2_t b)
{
    atom_vec2_t v = {a.x - b.x, a.y - b.y};
    return v;
}

atom_vec2_t atom_vec2_scale(atom_vec2_t v, float s)
{
    atom_vec2_t r = {v.x * s, v.y * s};
    return r;
}

float atom_vec2_dot(atom_vec2_t a, atom_vec2_t b)
{
    return a.x * b.x + a.y * b.y;
}

float atom_vec2_length(atom_vec2_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

float atom_vec2_length_sq(atom_vec2_t v)
{
    return v.x * v.x + v.y * v.y;
}

atom_vec2_t atom_vec2_normalize(atom_vec2_t v)
{
    float len = atom_vec2_length(v);
    if (len > ATOM_EPSILON)
    {
        float inv_len = 1.0f / len;
        atom_vec2_t r = {v.x * inv_len, v.y * inv_len};
        return r;
    }
    return atom_vec2_zero();
}

/* ============================================================================
 * Vec3 Implementation
 * ============================================================================ */

atom_vec3_t atom_vec3_create(float x, float y, float z)
{
    atom_vec3_t v = {x, y, z};
    return v;
}

atom_vec3_t atom_vec3_zero(void)
{
    atom_vec3_t v = {0.0f, 0.0f, 0.0f};
    return v;
}

atom_vec3_t atom_vec3_one(void)
{
    atom_vec3_t v = {1.0f, 1.0f, 1.0f};
    return v;
}

atom_vec3_t atom_vec3_up(void)
{
    atom_vec3_t v = {0.0f, 1.0f, 0.0f};
    return v;
}

atom_vec3_t atom_vec3_down(void)
{
    atom_vec3_t v = {0.0f, -1.0f, 0.0f};
    return v;
}

atom_vec3_t atom_vec3_forward(void)
{
    atom_vec3_t v = {0.0f, 0.0f, -1.0f};
    return v;
}

atom_vec3_t atom_vec3_back(void)
{
    atom_vec3_t v = {0.0f, 0.0f, 1.0f};
    return v;
}

atom_vec3_t atom_vec3_right(void)
{
    atom_vec3_t v = {1.0f, 0.0f, 0.0f};
    return v;
}

atom_vec3_t atom_vec3_left(void)
{
    atom_vec3_t v = {-1.0f, 0.0f, 0.0f};
    return v;
}

atom_vec3_t atom_vec3_add(atom_vec3_t a, atom_vec3_t b)
{
    atom_vec3_t v = {a.x + b.x, a.y + b.y, a.z + b.z};
    return v;
}

atom_vec3_t atom_vec3_sub(atom_vec3_t a, atom_vec3_t b)
{
    atom_vec3_t v = {a.x - b.x, a.y - b.y, a.z - b.z};
    return v;
}

atom_vec3_t atom_vec3_mul(atom_vec3_t a, atom_vec3_t b)
{
    atom_vec3_t v = {a.x * b.x, a.y * b.y, a.z * b.z};
    return v;
}

atom_vec3_t atom_vec3_div(atom_vec3_t a, atom_vec3_t b)
{
    atom_vec3_t v = {a.x / b.x, a.y / b.y, a.z / b.z};
    return v;
}

atom_vec3_t atom_vec3_scale(atom_vec3_t v, float s)
{
    atom_vec3_t r = {v.x * s, v.y * s, v.z * s};
    return r;
}

atom_vec3_t atom_vec3_negate(atom_vec3_t v)
{
    atom_vec3_t r = {-v.x, -v.y, -v.z};
    return r;
}

float atom_vec3_dot(atom_vec3_t a, atom_vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

atom_vec3_t atom_vec3_cross(atom_vec3_t a, atom_vec3_t b)
{
    atom_vec3_t v = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return v;
}

float atom_vec3_length(atom_vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float atom_vec3_length_sq(atom_vec3_t v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float atom_vec3_distance(atom_vec3_t a, atom_vec3_t b)
{
    return atom_vec3_length(atom_vec3_sub(b, a));
}

float atom_vec3_distance_sq(atom_vec3_t a, atom_vec3_t b)
{
    return atom_vec3_length_sq(atom_vec3_sub(b, a));
}

atom_vec3_t atom_vec3_normalize(atom_vec3_t v)
{
    float len = atom_vec3_length(v);
    if (len > ATOM_EPSILON)
    {
        float inv_len = 1.0f / len;
        atom_vec3_t r = {v.x * inv_len, v.y * inv_len, v.z * inv_len};
        return r;
    }
    return atom_vec3_zero();
}

bool atom_vec3_is_normalized(atom_vec3_t v)
{
    return atom_approx_equal(atom_vec3_length_sq(v), 1.0f, ATOM_EPSILON);
}

atom_vec3_t atom_vec3_lerp(atom_vec3_t a, atom_vec3_t b, float t)
{
    atom_vec3_t v = {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
    return v;
}

bool atom_vec3_equals(atom_vec3_t a, atom_vec3_t b, float epsilon)
{
    return atom_absf(a.x - b.x) <= epsilon && atom_absf(a.y - b.y) <= epsilon &&
           atom_absf(a.z - b.z) <= epsilon;
}

atom_vec3_t atom_vec3_reflect(atom_vec3_t v, atom_vec3_t normal)
{
    float d = 2.0f * atom_vec3_dot(v, normal);
    return atom_vec3_sub(v, atom_vec3_scale(normal, d));
}

atom_vec3_t atom_vec3_project(atom_vec3_t v, atom_vec3_t onto)
{
    float dot = atom_vec3_dot(v, onto);
    float len_sq = atom_vec3_length_sq(onto);
    if (len_sq > ATOM_EPSILON)
    {
        return atom_vec3_scale(onto, dot / len_sq);
    }
    return atom_vec3_zero();
}

/* ============================================================================
 * Vec4 Implementation
 * ============================================================================ */

atom_vec4_t atom_vec4_create(float x, float y, float z, float w)
{
    atom_vec4_t v = {x, y, z, w};
    return v;
}

atom_vec4_t atom_vec4_from_vec3(atom_vec3_t v, float w)
{
    atom_vec4_t r = {v.x, v.y, v.z, w};
    return r;
}

atom_vec3_t atom_vec4_to_vec3(atom_vec4_t v)
{
    atom_vec3_t r = {v.x, v.y, v.z};
    return r;
}

atom_vec4_t atom_vec4_zero(void)
{
    atom_vec4_t v = {0.0f, 0.0f, 0.0f, 0.0f};
    return v;
}

atom_vec4_t atom_vec4_add(atom_vec4_t a, atom_vec4_t b)
{
    atom_vec4_t v = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return v;
}

atom_vec4_t atom_vec4_sub(atom_vec4_t a, atom_vec4_t b)
{
    atom_vec4_t v = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return v;
}

atom_vec4_t atom_vec4_scale(atom_vec4_t v, float s)
{
    atom_vec4_t r = {v.x * s, v.y * s, v.z * s, v.w * s};
    return r;
}

float atom_vec4_dot(atom_vec4_t a, atom_vec4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float atom_vec4_length(atom_vec4_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

atom_vec4_t atom_vec4_normalize(atom_vec4_t v)
{
    float len = atom_vec4_length(v);
    if (len > ATOM_EPSILON)
    {
        float inv_len = 1.0f / len;
        atom_vec4_t r = {v.x * inv_len, v.y * inv_len, v.z * inv_len, v.w * inv_len};
        return r;
    }
    return atom_vec4_zero();
}

/* ============================================================================
 * Quaternion Implementation
 * ============================================================================ */

atom_quat_t atom_quat_identity(void)
{
    atom_quat_t q = {1.0f, 0.0f, 0.0f, 0.0f};
    return q;
}

atom_quat_t atom_quat_create(float w, float x, float y, float z)
{
    atom_quat_t q = {w, x, y, z};
    return q;
}

atom_quat_t atom_quat_from_axis_angle(atom_vec3_t axis, float angle_rad)
{
    float half_angle = angle_rad * 0.5f;
    float s = sinf(half_angle);
    atom_vec3_t n = atom_vec3_normalize(axis);
    atom_quat_t q = {cosf(half_angle), n.x * s, n.y * s, n.z * s};
    return q;
}

atom_quat_t atom_quat_from_euler(float pitch, float yaw, float roll)
{
    /* ZYX order (yaw, pitch, roll applied in that order) */
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);

    atom_quat_t q = {cr * cp * cy + sr * sp * sy, sr * cp * cy - cr * sp * sy,
                     cr * sp * cy + sr * cp * sy, cr * cp * sy - sr * sp * cy};
    return q;
}

atom_quat_t atom_quat_from_euler_deg(float pitch_deg, float yaw_deg, float roll_deg)
{
    return atom_quat_from_euler(pitch_deg * ATOM_DEG_TO_RAD, yaw_deg * ATOM_DEG_TO_RAD,
                                roll_deg * ATOM_DEG_TO_RAD);
}

atom_quat_t atom_quat_multiply(atom_quat_t a, atom_quat_t b)
{
    atom_quat_t q = {a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                     a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                     a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                     a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w};
    return q;
}

atom_quat_t atom_quat_conjugate(atom_quat_t q)
{
    atom_quat_t r = {q.w, -q.x, -q.y, -q.z};
    return r;
}

atom_quat_t atom_quat_inverse(atom_quat_t q)
{
    float len_sq = atom_quat_length_sq(q);
    if (len_sq > ATOM_EPSILON)
    {
        float inv_len_sq = 1.0f / len_sq;
        atom_quat_t r = {q.w * inv_len_sq, -q.x * inv_len_sq, -q.y * inv_len_sq, -q.z * inv_len_sq};
        return r;
    }
    return atom_quat_identity();
}

atom_quat_t atom_quat_normalize(atom_quat_t q)
{
    float len = atom_quat_length(q);
    if (len > ATOM_EPSILON)
    {
        float inv_len = 1.0f / len;
        atom_quat_t r = {q.w * inv_len, q.x * inv_len, q.y * inv_len, q.z * inv_len};
        return r;
    }
    return atom_quat_identity();
}

float atom_quat_length(atom_quat_t q)
{
    return sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

float atom_quat_length_sq(atom_quat_t q)
{
    return q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
}

float atom_quat_dot(atom_quat_t a, atom_quat_t b)
{
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

atom_vec3_t atom_quat_rotate_vec3(atom_quat_t q, atom_vec3_t v)
{
    /* Optimized quaternion-vector rotation: v' = q * v * q^-1 */
    atom_vec3_t qv = {q.x, q.y, q.z};
    atom_vec3_t uv = atom_vec3_cross(qv, v);
    atom_vec3_t uuv = atom_vec3_cross(qv, uv);

    uv = atom_vec3_scale(uv, 2.0f * q.w);
    uuv = atom_vec3_scale(uuv, 2.0f);

    return atom_vec3_add(atom_vec3_add(v, uv), uuv);
}

atom_quat_t atom_quat_lerp(atom_quat_t a, atom_quat_t b, float t)
{
    atom_quat_t q = {a.w + (b.w - a.w) * t, a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
                     a.z + (b.z - a.z) * t};
    return atom_quat_normalize(q);
}

atom_quat_t atom_quat_slerp(atom_quat_t a, atom_quat_t b, float t)
{
    float dot = atom_quat_dot(a, b);

    /* If dot is negative, negate one quaternion to take shortest path */
    if (dot < 0.0f)
    {
        b.w = -b.w;
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        dot = -dot;
    }

    /* If quaternions are very close, use linear interpolation */
    if (dot > 0.9995f)
    {
        return atom_quat_lerp(a, b, t);
    }

    float theta_0 = acosf(dot);
    float theta = theta_0 * t;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    atom_quat_t q = {a.w * s0 + b.w * s1, a.x * s0 + b.x * s1, a.y * s0 + b.y * s1,
                     a.z * s0 + b.z * s1};
    return q;
}

void atom_quat_to_euler(atom_quat_t q, float *pitch, float *yaw, float *roll)
{
    /* Extract Euler angles from quaternion (ZYX order) */
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (atom_absf(sinp) >= 1.0f)
    {
        *pitch = copysignf(ATOM_HALF_PI, sinp);
    }
    else
    {
        *pitch = asinf(sinp);
    }

    float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    *yaw = atan2f(siny_cosp, cosy_cosp);
}

void atom_quat_to_axis_angle(atom_quat_t q, atom_vec3_t *axis, float *angle)
{
    q = atom_quat_normalize(q);

    *angle = 2.0f * acosf(atom_clampf(q.w, -1.0f, 1.0f));

    float s = sqrtf(1.0f - q.w * q.w);
    if (s < ATOM_EPSILON)
    {
        axis->x = 1.0f;
        axis->y = 0.0f;
        axis->z = 0.0f;
    }
    else
    {
        float inv_s = 1.0f / s;
        axis->x = q.x * inv_s;
        axis->y = q.y * inv_s;
        axis->z = q.z * inv_s;
    }
}

atom_vec3_t atom_quat_get_forward(atom_quat_t q)
{
    return atom_quat_rotate_vec3(q, atom_vec3_forward());
}

atom_vec3_t atom_quat_get_right(atom_quat_t q)
{
    return atom_quat_rotate_vec3(q, atom_vec3_right());
}

atom_vec3_t atom_quat_get_up(atom_quat_t q)
{
    return atom_quat_rotate_vec3(q, atom_vec3_up());
}

bool atom_quat_equals(atom_quat_t a, atom_quat_t b, float epsilon)
{
    return atom_absf(a.w - b.w) <= epsilon && atom_absf(a.x - b.x) <= epsilon &&
           atom_absf(a.y - b.y) <= epsilon && atom_absf(a.z - b.z) <= epsilon;
}

/* ============================================================================
 * Mat4 Implementation
 * ============================================================================ */

atom_mat4_t atom_mat4_identity(void)
{
    atom_mat4_t m = {.m = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f}};
    return m;
}

atom_mat4_t atom_mat4_zero(void)
{
    atom_mat4_t m = {.m = {0}};
    return m;
}

atom_mat4_t atom_mat4_from_quat(atom_quat_t q)
{
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    atom_mat4_t m = {.m = {1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
                           2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f,
                           2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f}};
    return m;
}

atom_mat4_t atom_mat4_from_cols(atom_vec4_t c0, atom_vec4_t c1, atom_vec4_t c2, atom_vec4_t c3)
{
    atom_mat4_t m = {.m = {c0.x, c0.y, c0.z, c0.w, c1.x, c1.y, c1.z, c1.w, c2.x, c2.y, c2.z, c2.w,
                           c3.x, c3.y, c3.z, c3.w}};
    return m;
}

atom_mat4_t atom_mat4_translate(atom_vec3_t translation)
{
    atom_mat4_t m = atom_mat4_identity();
    m.m[12] = translation.x;
    m.m[13] = translation.y;
    m.m[14] = translation.z;
    return m;
}

atom_mat4_t atom_mat4_scale(atom_vec3_t scale)
{
    atom_mat4_t m = atom_mat4_identity();
    m.m[0] = scale.x;
    m.m[5] = scale.y;
    m.m[10] = scale.z;
    return m;
}

atom_mat4_t atom_mat4_scale_uniform(float scale)
{
    return atom_mat4_scale(atom_vec3_create(scale, scale, scale));
}

atom_mat4_t atom_mat4_rotate_x(float angle_rad)
{
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    atom_mat4_t m = atom_mat4_identity();
    m.m[5] = c;
    m.m[6] = s;
    m.m[9] = -s;
    m.m[10] = c;
    return m;
}

atom_mat4_t atom_mat4_rotate_y(float angle_rad)
{
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    atom_mat4_t m = atom_mat4_identity();
    m.m[0] = c;
    m.m[2] = -s;
    m.m[8] = s;
    m.m[10] = c;
    return m;
}

atom_mat4_t atom_mat4_rotate_z(float angle_rad)
{
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    atom_mat4_t m = atom_mat4_identity();
    m.m[0] = c;
    m.m[1] = s;
    m.m[4] = -s;
    m.m[5] = c;
    return m;
}

atom_mat4_t atom_mat4_rotate_axis(atom_vec3_t axis, float angle_rad)
{
    return atom_mat4_from_quat(atom_quat_from_axis_angle(axis, angle_rad));
}

atom_mat4_t atom_mat4_rotate_quat(atom_quat_t q)
{
    return atom_mat4_from_quat(q);
}

atom_mat4_t atom_mat4_look_at(atom_vec3_t eye, atom_vec3_t target, atom_vec3_t up)
{
    atom_vec3_t f = atom_vec3_normalize(atom_vec3_sub(target, eye));
    atom_vec3_t r = atom_vec3_normalize(atom_vec3_cross(f, up));
    atom_vec3_t u = atom_vec3_cross(r, f);

    atom_mat4_t m = {.m = {r.x, u.x, -f.x, 0.0f, r.y, u.y, -f.y, 0.0f, r.z, u.z, -f.z, 0.0f,
                           -atom_vec3_dot(r, eye), -atom_vec3_dot(u, eye), atom_vec3_dot(f, eye),
                           1.0f}};
    return m;
}

atom_mat4_t atom_mat4_perspective(float fov_rad, float aspect, float near_plane, float far_plane)
{
    float tan_half_fov = tanf(fov_rad * 0.5f);

    atom_mat4_t m = atom_mat4_zero();
    m.m[0] = 1.0f / (aspect * tan_half_fov);
    m.m[5] = 1.0f / tan_half_fov;
    m.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    m.m[11] = -1.0f;
    m.m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return m;
}

atom_mat4_t atom_mat4_perspective_deg(float fov_deg, float aspect, float near_plane,
                                      float far_plane)
{
    return atom_mat4_perspective(fov_deg * ATOM_DEG_TO_RAD, aspect, near_plane, far_plane);
}

atom_mat4_t atom_mat4_ortho(float left, float right, float bottom, float top, float near_plane,
                            float far_plane)
{
    atom_mat4_t m = atom_mat4_identity();
    m.m[0] = 2.0f / (right - left);
    m.m[5] = 2.0f / (top - bottom);
    m.m[10] = -2.0f / (far_plane - near_plane);
    m.m[12] = -(right + left) / (right - left);
    m.m[13] = -(top + bottom) / (top - bottom);
    m.m[14] = -(far_plane + near_plane) / (far_plane - near_plane);
    return m;
}

atom_mat4_t atom_mat4_multiply(atom_mat4_t a, atom_mat4_t b)
{
    atom_mat4_t m;

    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            m.m[col * 4 + row] =
                a.m[0 * 4 + row] * b.m[col * 4 + 0] + a.m[1 * 4 + row] * b.m[col * 4 + 1] +
                a.m[2 * 4 + row] * b.m[col * 4 + 2] + a.m[3 * 4 + row] * b.m[col * 4 + 3];
        }
    }

    return m;
}

atom_vec4_t atom_mat4_multiply_vec4(atom_mat4_t m, atom_vec4_t v)
{
    atom_vec4_t r = {m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w,
                     m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w,
                     m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
                     m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w};
    return r;
}

atom_vec3_t atom_mat4_transform_point(atom_mat4_t m, atom_vec3_t p)
{
    atom_vec4_t v = atom_mat4_multiply_vec4(m, atom_vec4_from_vec3(p, 1.0f));
    if (atom_absf(v.w) > ATOM_EPSILON)
    {
        float inv_w = 1.0f / v.w;
        return atom_vec3_create(v.x * inv_w, v.y * inv_w, v.z * inv_w);
    }
    return atom_vec4_to_vec3(v);
}

atom_vec3_t atom_mat4_transform_direction(atom_mat4_t m, atom_vec3_t d)
{
    atom_vec4_t v = atom_mat4_multiply_vec4(m, atom_vec4_from_vec3(d, 0.0f));
    return atom_vec4_to_vec3(v);
}

atom_mat4_t atom_mat4_transpose(atom_mat4_t m)
{
    atom_mat4_t r = {.m = {m.m[0], m.m[4], m.m[8], m.m[12], m.m[1], m.m[5], m.m[9], m.m[13], m.m[2],
                           m.m[6], m.m[10], m.m[14], m.m[3], m.m[7], m.m[11], m.m[15]}};
    return r;
}

float atom_mat4_determinant(atom_mat4_t m)
{
    float a = m.m[0], b = m.m[1], c = m.m[2], d = m.m[3];
    float e = m.m[4], f = m.m[5], g = m.m[6], h = m.m[7];
    float i = m.m[8], j = m.m[9], k = m.m[10], l = m.m[11];
    float n = m.m[12], o = m.m[13], p = m.m[14], q = m.m[15];

    float kq_lp = k * q - l * p;
    float jq_lo = j * q - l * o;
    float jp_ko = j * p - k * o;
    float iq_ln = i * q - l * n;
    float ip_kn = i * p - k * n;
    float io_jn = i * o - j * n;

    return a * (f * kq_lp - g * jq_lo + h * jp_ko) - b * (e * kq_lp - g * iq_ln + h * ip_kn) +
           c * (e * jq_lo - f * iq_ln + h * io_jn) - d * (e * jp_ko - f * ip_kn + g * io_jn);
}

atom_mat4_t atom_mat4_inverse(atom_mat4_t m)
{
    float a = m.m[0], b = m.m[1], c = m.m[2], d = m.m[3];
    float e = m.m[4], f = m.m[5], g = m.m[6], h = m.m[7];
    float i = m.m[8], j = m.m[9], k = m.m[10], l = m.m[11];
    float n = m.m[12], o = m.m[13], p = m.m[14], q = m.m[15];

    float kq_lp = k * q - l * p;
    float jq_lo = j * q - l * o;
    float jp_ko = j * p - k * o;
    float iq_ln = i * q - l * n;
    float ip_kn = i * p - k * n;
    float io_jn = i * o - j * n;
    float gq_hp = g * q - h * p;
    float fq_ho = f * q - h * o;
    float fp_go = f * p - g * o;
    float eq_hn = e * q - h * n;
    float ep_gn = e * p - g * n;
    float eo_fn = e * o - f * n;
    float gk_hl = g * l - h * k;
    float fk_hj = f * l - h * j;
    float fk_gj = f * k - g * j;
    float ek_hi = e * l - h * i;
    float ek_gi = e * k - g * i;
    float ej_fi = e * j - f * i;

    float det = a * (f * kq_lp - g * jq_lo + h * jp_ko) - b * (e * kq_lp - g * iq_ln + h * ip_kn) +
                c * (e * jq_lo - f * iq_ln + h * io_jn) - d * (e * jp_ko - f * ip_kn + g * io_jn);

    if (atom_absf(det) < ATOM_EPSILON)
    {
        return atom_mat4_identity();
    }

    float inv_det = 1.0f / det;

    atom_mat4_t r;
    r.m[0] = (f * kq_lp - g * jq_lo + h * jp_ko) * inv_det;
    r.m[1] = -(b * kq_lp - c * jq_lo + d * jp_ko) * inv_det;
    r.m[2] = (b * gq_hp - c * fq_ho + d * fp_go) * inv_det;
    r.m[3] = -(b * gk_hl - c * fk_hj + d * fk_gj) * inv_det;

    r.m[4] = -(e * kq_lp - g * iq_ln + h * ip_kn) * inv_det;
    r.m[5] = (a * kq_lp - c * iq_ln + d * ip_kn) * inv_det;
    r.m[6] = -(a * gq_hp - c * eq_hn + d * ep_gn) * inv_det;
    r.m[7] = (a * gk_hl - c * ek_hi + d * ek_gi) * inv_det;

    r.m[8] = (e * jq_lo - f * iq_ln + h * io_jn) * inv_det;
    r.m[9] = -(a * jq_lo - b * iq_ln + d * io_jn) * inv_det;
    r.m[10] = (a * fq_ho - b * eq_hn + d * eo_fn) * inv_det;
    r.m[11] = -(a * fk_hj - b * ek_hi + d * ej_fi) * inv_det;

    r.m[12] = -(e * jp_ko - f * ip_kn + g * io_jn) * inv_det;
    r.m[13] = (a * jp_ko - b * ip_kn + c * io_jn) * inv_det;
    r.m[14] = -(a * fp_go - b * ep_gn + c * eo_fn) * inv_det;
    r.m[15] = (a * fk_gj - b * ek_gi + c * ej_fi) * inv_det;

    return r;
}

atom_vec3_t atom_mat4_get_translation(atom_mat4_t m)
{
    return atom_vec3_create(m.m[12], m.m[13], m.m[14]);
}

atom_vec3_t atom_mat4_get_scale(atom_mat4_t m)
{
    atom_vec3_t sx = {m.m[0], m.m[1], m.m[2]};
    atom_vec3_t sy = {m.m[4], m.m[5], m.m[6]};
    atom_vec3_t sz = {m.m[8], m.m[9], m.m[10]};
    return atom_vec3_create(atom_vec3_length(sx), atom_vec3_length(sy), atom_vec3_length(sz));
}

atom_quat_t atom_mat4_get_rotation(atom_mat4_t m)
{
    atom_vec3_t scale = atom_mat4_get_scale(m);

    /* Remove scale from matrix */
    float m00 = m.m[0] / scale.x, m01 = m.m[4] / scale.y, m02 = m.m[8] / scale.z;
    float m10 = m.m[1] / scale.x, m11 = m.m[5] / scale.y, m12 = m.m[9] / scale.z;
    float m20 = m.m[2] / scale.x, m21 = m.m[6] / scale.y, m22 = m.m[10] / scale.z;

    atom_quat_t q;
    float trace = m00 + m11 + m22;

    if (trace > 0.0f)
    {
        float s = 0.5f / sqrtf(trace + 1.0f);
        q.w = 0.25f / s;
        q.x = (m21 - m12) * s;
        q.y = (m02 - m20) * s;
        q.z = (m10 - m01) * s;
    }
    else if (m00 > m11 && m00 > m22)
    {
        float s = 2.0f * sqrtf(1.0f + m00 - m11 - m22);
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    }
    else if (m11 > m22)
    {
        float s = 2.0f * sqrtf(1.0f + m11 - m00 - m22);
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    }
    else
    {
        float s = 2.0f * sqrtf(1.0f + m22 - m00 - m11);
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    return atom_quat_normalize(q);
}

float atom_mat4_get(atom_mat4_t m, int row, int col)
{
    return m.m[col * 4 + row];
}

void atom_mat4_set(atom_mat4_t *m, int row, int col, float value)
{
    m->m[col * 4 + row] = value;
}
