#pragma once

#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include "engine/ecs/Components.hpp"

namespace engine {

class RigidBody2DUtils {
public:
    static void applyLinearImpulse(b2BodyId bodyId, glm::vec2 impulse, bool wake = true);
    static void setLinearVelocity(b2BodyId bodyId, glm::vec2 velocity);
    static glm::vec2 getLinearVelocity(b2BodyId bodyId);
    static glm::vec2 getPosition(b2BodyId bodyId);
};

} // namespace engine
