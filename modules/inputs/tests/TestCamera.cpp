/**
 * @file TestCamera.cpp
 * @brief Unit tests for Atom::Input::Camera, FPSCamera, and OrbitalCamera.
 */

#include "TestHelpers.hpp"
#include "inputs/Camera.hpp"
#include "inputs/FPSCamera.hpp"
#include "inputs/OrbitalCamera.hpp"
#include "inputs/InputState.hpp"

#include <cstring>

using namespace Atom::Input;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Build a KeyDown event for a specific key.
static Event makeKeyDown(KeyCode key, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type          = EventType::KeyDown;
    e.timestampMs   = ts;
    e.key.key       = key;
    e.key.isRepeat  = false;
    e.key.modifiers = 0;
    return e;
}

/// Build a MouseScroll event.
static Event makeMouseScroll(float dx, float dy, uint64_t ts = 0)
{
    Event e{};
    std::memset(&e, 0, sizeof(e));
    e.type               = EventType::MouseScroll;
    e.timestampMs        = ts;
    e.mouseScroll.deltaX = dx;
    e.mouseScroll.deltaY = dy;
    return e;
}

// =========================================================================
// Camera core tests
// =========================================================================

static int test_camera_default()
{
    Camera cam;

    glm::vec3 pos = cam.position();
    ASSERT_VEC3_NEAR(pos, glm::vec3(0.0f, 0.0f, 5.0f), 0.01f);

    glm::vec3 fwd = cam.forward();
    ASSERT_VEC3_NEAR(fwd, glm::vec3(0.0f, 0.0f, -1.0f), 0.01f);

    return 0;
}

static int test_camera_set_position()
{
    Camera cam;
    cam.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));

    glm::vec3 pos = cam.position();
    ASSERT_VEC3_NEAR(pos, glm::vec3(1.0f, 2.0f, 3.0f), 0.001f);

    return 0;
}

static int test_camera_set_euler()
{
    Camera cam;

    // Pitch = 0, Yaw = 90 degrees => looking right (+X).
    cam.setEuler(0.0f, 90.0f);

    glm::vec3 fwd = cam.forward();
    ASSERT_VEC3_NEAR(fwd, glm::vec3(1.0f, 0.0f, 0.0f), 0.02f);

    return 0;
}

static int test_camera_look_at()
{
    Camera cam;
    cam.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    cam.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    glm::vec3 fwd = cam.forward();
    ASSERT_VEC3_NEAR(fwd, glm::vec3(0.0f, 0.0f, -1.0f), 0.02f);

    return 0;
}

static int test_camera_perspective()
{
    Camera cam;
    cam.setPerspective(90.0f, 1.0f, 0.1f, 100.0f);

    const glm::mat4& proj = cam.projectionMatrix();
    glm::mat4 identity(1.0f);

    // The projection matrix should NOT be the identity.
    bool isIdentity = true;
    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            if (std::fabs(proj[c][r] - identity[c][r]) > 0.001f)
            {
                isIdentity = false;
            }
        }
    }
    ASSERT_FALSE(isIdentity);

    return 0;
}

static int test_camera_view_matrix()
{
    Camera cam;

    const glm::mat4& view1 = cam.viewMatrix();

    // View matrix should not be identity (camera is at (0,0,5)).
    glm::mat4 identity(1.0f);
    bool isIdentity = true;
    for (int c = 0; c < 4; ++c)
    {
        for (int r = 0; r < 4; ++r)
        {
            if (std::fabs(view1[c][r] - identity[c][r]) > 0.001f)
            {
                isIdentity = false;
            }
        }
    }
    ASSERT_FALSE(isIdentity);

    // Move the camera and verify the view matrix changes.
    // Store a value from the old matrix.
    float oldVal = view1[3][2]; // translation component

    cam.setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    const glm::mat4& view2 = cam.viewMatrix();

    // At least one element should have changed.
    bool changed = false;
    for (int c = 0; c < 4 && !changed; ++c)
    {
        for (int r = 0; r < 4 && !changed; ++r)
        {
            if (std::fabs(view2[c][r] - view1[c][r]) > 0.001f)
            {
                changed = true;
            }
        }
    }
    // Note: view1 and view2 are the same reference, so we compare to
    // the saved value.
    ASSERT_TRUE(std::fabs(view2[3][2] - oldVal) > 0.001f
                || std::fabs(view2[3][0]) > 0.001f);

    return 0;
}

static int test_camera_mode_switch()
{
    Camera cam(CameraMode::FPS);
    ASSERT_EQ(cam.mode(), CameraMode::FPS);

    cam.setMode(CameraMode::Orbital);
    ASSERT_EQ(cam.mode(), CameraMode::Orbital);

    return 0;
}

// =========================================================================
// FPSCamera tests
// =========================================================================

static int test_fps_camera_movement()
{
    Camera cam(CameraMode::FPS);
    glm::vec3 startPos = cam.position(); // (0, 0, 5)

    // Simulate one frame with W pressed (forward).
    InputState input;
    input.beginFrame(0);
    input.processEvent(makeKeyDown(KeyCode::W, 0));
    input.endFrame();

    FPSCamera fps;
    float dt = 1.0f / 60.0f;
    fps.update(cam, input, dt);

    glm::vec3 endPos = cam.position();

    // Camera should have moved forward (negative Z direction by default).
    // forward() is (0, 0, -1) at default orientation, and W maps to +Z
    // in local space which becomes forward direction.
    float dist = glm::length(endPos - startPos);
    ASSERT_TRUE(dist > 0.001f);

    return 0;
}

// =========================================================================
// OrbitalCamera tests
// =========================================================================

static int test_orbital_camera_zoom()
{
    Camera cam(CameraMode::Orbital);
    cam.forceUpdate();

    float startDist = cam.orbitDistance(); // default: 10.0

    // Simulate a scroll event.
    InputState input;
    input.beginFrame(0);
    input.processEvent(makeMouseScroll(0.0f, 1.0f));
    input.endFrame();

    OrbitalCamera orbital;
    orbital.processZoom(cam, input);

    float endDist = cam.orbitDistance();

    // Scrolling positive Y should change the orbit distance.
    ASSERT_TRUE(std::fabs(endDist - startDist) > 0.001f);

    return 0;
}

// =========================================================================
// main
// =========================================================================

int main()
{
    int failures = 0;

    std::printf("TestCamera\n");

    RUN_TEST(test_camera_default);
    RUN_TEST(test_camera_set_position);
    RUN_TEST(test_camera_set_euler);
    RUN_TEST(test_camera_look_at);
    RUN_TEST(test_camera_perspective);
    RUN_TEST(test_camera_view_matrix);
    RUN_TEST(test_camera_mode_switch);
    RUN_TEST(test_fps_camera_movement);
    RUN_TEST(test_orbital_camera_zoom);

    std::printf("\n  %d failure(s)\n", failures);
    return failures;
}
