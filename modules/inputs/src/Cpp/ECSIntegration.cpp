/**
 * @file ECSIntegration.cpp
 * @brief Implementation of InputECSSystem and CameraECSSystem.
 */

#include "inputs/ECSIntegration.hpp"

namespace Atom::Input
{

// ---------------------------------------------------------------------------
// InputECSSystem
// ---------------------------------------------------------------------------

void InputECSSystem::Update(ECS::ECS &ecs, ECS::SystemID /*thisID*/, uint32_t msecs)
{
    auto entities = ecs.getEntitiesByComponentsAllOf<InputComponent>();

    for (ECS::EntityID id : entities)
    {
        InputComponent &input = ecs.entityGetComponent<InputComponent>(id);
        input.system.update(static_cast<uint64_t>(msecs));
    }
}

// ---------------------------------------------------------------------------
// CameraECSSystem
// ---------------------------------------------------------------------------

void CameraECSSystem::Update(ECS::ECS &ecs, ECS::SystemID /*thisID*/, uint32_t msecs)
{
    // -- Locate the first InputComponent to read input state from. ----------

    auto inputEntities = ecs.getEntitiesByComponentsAllOf<InputComponent>();

    if (inputEntities.empty())
    {
        return;
    }

    ECS::EntityID inputEntity = inputEntities.front();
    const InputState &inputState =
        ecs.entityGetComponent<InputComponent>(inputEntity).system.state();

    float dt = static_cast<float>(msecs) / 1000.0f;

    // -- Update every enabled CameraComponent. ------------------------------

    auto camEntities = ecs.getEntitiesByComponentsAllOf<CameraComponent>();

    for (ECS::EntityID id : camEntities)
    {
        CameraComponent &cam = ecs.entityGetComponent<CameraComponent>(id);

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
