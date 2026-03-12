/**
 * @file TestHelpers.hpp
 * @brief Lightweight C++ test macros for the ATOM input system unit tests.
 */

#ifndef ATOM_INPUT_TESTHELPERS_HPP
#define ATOM_INPUT_TESTHELPERS_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

// ---------------------------------------------------------------------------
// Test runner
// ---------------------------------------------------------------------------

/**
 * @brief Run a single test function, printing its name and result.
 *
 * The test function must return 0 on success or non-zero on failure.
 */
#define RUN_TEST(fn)                                                        \
    do                                                                      \
    {                                                                       \
        std::printf("  %-50s ", #fn);                                       \
        int result_ = fn();                                                 \
        if (result_ != 0)                                                   \
        {                                                                   \
            failures++;                                                     \
        }                                                                   \
        else                                                                \
        {                                                                   \
            std::printf("PASSED\n");                                        \
        }                                                                   \
    } while (0)

// ---------------------------------------------------------------------------
// Assertions
// ---------------------------------------------------------------------------

/**
 * @brief Assert that @p expr evaluates to true.
 */
#define ASSERT_TRUE(expr)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(expr))                                                        \
        {                                                                   \
            std::printf("FAILED\n    %s:%d: ASSERT_TRUE(%s)\n",             \
                        __FILE__, __LINE__, #expr);                         \
            return 1;                                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Assert that @p expr evaluates to false.
 */
#define ASSERT_FALSE(expr)                                                  \
    do                                                                      \
    {                                                                       \
        if ((expr))                                                         \
        {                                                                   \
            std::printf("FAILED\n    %s:%d: ASSERT_FALSE(%s)\n",            \
                        __FILE__, __LINE__, #expr);                         \
            return 1;                                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Assert that @p a equals @p b.
 */
#define ASSERT_EQ(a, b)                                                     \
    do                                                                      \
    {                                                                       \
        if ((a) != (b))                                                     \
        {                                                                   \
            std::printf("FAILED\n    %s:%d: ASSERT_EQ(%s, %s)\n",          \
                        __FILE__, __LINE__, #a, #b);                        \
            return 1;                                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Assert that two floating-point values are within @p eps of each
 *        other.
 */
#define ASSERT_FLOAT_NEAR(a, b, eps)                                        \
    do                                                                      \
    {                                                                       \
        if (std::fabs((a) - (b)) > (eps))                                   \
        {                                                                   \
            std::printf("FAILED\n    %s:%d: ASSERT_FLOAT_NEAR(%s, %s, %s)" \
                        "  [%.8f vs %.8f]\n",                               \
                        __FILE__, __LINE__, #a, #b, #eps,                   \
                        static_cast<double>(a), static_cast<double>(b));    \
            return 1;                                                       \
        }                                                                   \
    } while (0)

/**
 * @brief Assert that two glm::vec3 values are component-wise within @p eps.
 */
#define ASSERT_VEC3_NEAR(a, b, eps)                                         \
    do                                                                      \
    {                                                                       \
        glm::vec3 a_ = (a);                                                 \
        glm::vec3 b_ = (b);                                                 \
        if (!glm::all(glm::epsilonEqual(a_, b_, glm::vec3(eps))))           \
        {                                                                   \
            std::printf("FAILED\n    %s:%d: ASSERT_VEC3_NEAR(%s, %s, %s)"  \
                        "  [(%.4f,%.4f,%.4f) vs (%.4f,%.4f,%.4f)]\n",      \
                        __FILE__, __LINE__, #a, #b, #eps,                   \
                        static_cast<double>(a_.x),                          \
                        static_cast<double>(a_.y),                          \
                        static_cast<double>(a_.z),                          \
                        static_cast<double>(b_.x),                          \
                        static_cast<double>(b_.y),                          \
                        static_cast<double>(b_.z));                         \
            return 1;                                                       \
        }                                                                   \
    } while (0)

#endif // ATOM_INPUT_TESTHELPERS_HPP
