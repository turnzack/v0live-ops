#pragma once

#include "engine/ecs/World.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/physics2d/PhysicsWorld2D.hpp"

namespace engine {

class CharacterController2D {
public:
    void update(
        World& world,
        InputManager& input,
        PhysicsWorld2D& physics,
        float fixedDelta
    );

private:
    void updateTimers(
        CharacterController2DComponent& controller,
        float fixedDelta
    );
};

} // namespace engine
