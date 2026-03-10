/**
 * @file test_helpers.h
 * @brief Lightweight C11 test assertion macros for ATOM modules
 */

#ifndef ATOM_TEST_HELPERS_H_
#define ATOM_TEST_HELPERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_EPSILON 1e-5f

#define RUN_TEST(fn)                                                    \
    do {                                                                \
        g_tests_run++;                                                  \
        printf("  [RUN ] %s\n", #fn);                                   \
        fn();                                                           \
        g_tests_passed++;                                               \
        printf("  [PASS] %s\n", #fn);                                   \
    } while (0)

#define TEST_SUMMARY()                                                  \
    do {                                                                \
        printf("\n========================================\n");         \
        printf("Tests run:    %d\n", g_tests_run);                      \
        printf("Tests passed: %d\n", g_tests_passed);                   \
        printf("Tests failed: %d\n", g_tests_failed);                   \
        printf("========================================\n");           \
        return g_tests_failed > 0 ? 1 : 0;                             \
    } while (0)

#define ASSERT_TRUE(expr)                                               \
    do {                                                                \
        if (!(expr)) {                                                  \
            printf("  [FAIL] %s:%d: ASSERT_TRUE(%s) failed\n",         \
                   __FILE__, __LINE__, #expr);                          \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_FALSE(expr)                                              \
    do {                                                                \
        if ((expr)) {                                                   \
            printf("  [FAIL] %s:%d: ASSERT_FALSE(%s) failed\n",        \
                   __FILE__, __LINE__, #expr);                          \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_EQ(a, b)                                                 \
    do {                                                                \
        if ((a) != (b)) {                                               \
            printf("  [FAIL] %s:%d: ASSERT_EQ(%s, %s) failed\n",       \
                   __FILE__, __LINE__, #a, #b);                         \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_NEQ(a, b)                                                \
    do {                                                                \
        if ((a) == (b)) {                                               \
            printf("  [FAIL] %s:%d: ASSERT_NEQ(%s, %s) failed\n",      \
                   __FILE__, __LINE__, #a, #b);                         \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_FLOAT_EQ(a, b)                                           \
    do {                                                                \
        float _a = (a), _b = (b);                                       \
        if (fabsf(_a - _b) > TEST_EPSILON) {                            \
            printf("  [FAIL] %s:%d: ASSERT_FLOAT_EQ(%s, %s) "          \
                   "got %f vs %f\n",                                    \
                   __FILE__, __LINE__, #a, #b, (double)_a, (double)_b); \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_FLOAT_NEAR(a, b, eps)                                    \
    do {                                                                \
        float _a = (a), _b = (b);                                       \
        if (fabsf(_a - _b) > (eps)) {                                   \
            printf("  [FAIL] %s:%d: ASSERT_FLOAT_NEAR(%s, %s, %s) "    \
                   "got %f vs %f (delta %f)\n",                         \
                   __FILE__, __LINE__, #a, #b, #eps,                    \
                   (double)_a, (double)_b,                              \
                   (double)fabsf(_a - _b));                             \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_VEC3_EQ(a, b)                                            \
    do {                                                                \
        atom_vec3_t _va = (a), _vb = (b);                               \
        if (fabsf(_va.x - _vb.x) > TEST_EPSILON ||                     \
            fabsf(_va.y - _vb.y) > TEST_EPSILON ||                     \
            fabsf(_va.z - _vb.z) > TEST_EPSILON) {                     \
            printf("  [FAIL] %s:%d: ASSERT_VEC3_EQ(%s, %s) "           \
                   "got (%.6f,%.6f,%.6f) vs (%.6f,%.6f,%.6f)\n",       \
                   __FILE__, __LINE__, #a, #b,                          \
                   (double)_va.x, (double)_va.y, (double)_va.z,        \
                   (double)_vb.x, (double)_vb.y, (double)_vb.z);       \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_VEC3_NEAR(a, b, eps)                                     \
    do {                                                                \
        atom_vec3_t _va = (a), _vb = (b);                               \
        if (fabsf(_va.x - _vb.x) > (eps) ||                            \
            fabsf(_va.y - _vb.y) > (eps) ||                            \
            fabsf(_va.z - _vb.z) > (eps)) {                            \
            printf("  [FAIL] %s:%d: ASSERT_VEC3_NEAR(%s, %s, %s) "     \
                   "got (%.6f,%.6f,%.6f) vs (%.6f,%.6f,%.6f)\n",       \
                   __FILE__, __LINE__, #a, #b, #eps,                    \
                   (double)_va.x, (double)_va.y, (double)_va.z,        \
                   (double)_vb.x, (double)_vb.y, (double)_vb.z);       \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#define ASSERT_QUAT_NEAR(a, b, eps)                                     \
    do {                                                                \
        atom_quat_t _qa = (a), _qb = (b);                              \
        float _dot = _qa.w*_qb.w + _qa.x*_qb.x +                      \
                     _qa.y*_qb.y + _qa.z*_qb.z;                        \
        if (fabsf(fabsf(_dot) - 1.0f) > (eps)) {                       \
            printf("  [FAIL] %s:%d: ASSERT_QUAT_NEAR(%s, %s) "         \
                   "got (%.4f,%.4f,%.4f,%.4f) vs (%.4f,%.4f,%.4f,%.4f)" \
                   " dot=%.6f\n",                                       \
                   __FILE__, __LINE__, #a, #b,                          \
                   (double)_qa.w, (double)_qa.x,                        \
                   (double)_qa.y, (double)_qa.z,                        \
                   (double)_qb.w, (double)_qb.x,                        \
                   (double)_qb.y, (double)_qb.z,                        \
                   (double)_dot);                                       \
            g_tests_failed++;                                           \
            g_tests_run++;                                              \
            return;                                                     \
        }                                                               \
    } while (0)

#endif /* !ATOM_TEST_HELPERS_H_ */
