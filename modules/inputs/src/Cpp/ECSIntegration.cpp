/**
 * @file ECSIntegration.cpp
 * @brief Implementation of InputECSSystem and CameraECSSystem.
 */

#include "inputs/ECSIntegration.hpp"
#include "core/ECS/Includes.hpp"
#include "inputs/InputState.hpp"
#include "inputs/InputSystem.hpp"

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// InputECSSystem
// ---------------------------------------------------------------------------

void InputECSSystem::Update([[maybe_unused]]ECS::ECS &ecs, uint32_t msecs)
{
    m_input_system->update(static_cast<uint64_t>(msecs));
}

std::shared_ptr<InputSystem> InputECSSystem::inputSystem() const
{
    return m_input_system;
}

#ifdef ATOM_INPUT_HAS_GLFW

void InputECSSystem::bindGLFW(GLFWwindow *window)
{
    m_bridge = std::make_unique<GLFWBridge>(window, m_input_system->eventQueue());
}

void InputECSSystem::setCursorCaptured(bool captured)
{
    if (m_bridge)
        m_bridge->setCursorCaptured(captured);
}

#endif

// ---------------------------------------------------------------------------
// CameraECSSystem
// ---------------------------------------------------------------------------

CameraECSSystem::CameraECSSystem(std::shared_ptr<InputSystem> input)
: m_input(std::move(input))
{
}

void CameraECSSystem::Update(ECS::ECS &ecs, uint32_t msecs)
{
    const InputState &inputState = m_input->state();
    float dt = static_cast<float>(msecs) / 1000.0f;

    // -- Update every enabled CameraComponent. ------------------------------

    std::vector<ECS::EntityID> camEntities;
    ecs.getEntitiesByComponentsAllOf<CameraComponent>(camEntities);

    for (ECS::EntityID id : camEntities)
    {
        auto &cam = ecs.entityGetComponent<CameraComponent>(id);

        if (!cam.enabled)
        {
            continue;
        }

        switch (cam.mode)
        {
        case ControlMode::FPS:
            m_fps.update(cam.camera, inputState, dt, cam.fpsBindings);
            break;
        case ControlMode::Orbital:
            m_orbital.update(cam.camera, inputState, dt, cam.orbitalBindings);
            break;
        }
    }
}

} // namespace Atom::Input
