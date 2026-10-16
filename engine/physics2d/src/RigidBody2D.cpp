#include "engine/physics2d/RigidBody2D.hpp"

namespace engine {

void RigidBody2DUtils::applyLinearImpulse(b2BodyId bodyId, glm::vec2 impulse, bool wake) {
    if (B2_IS_NON_NULL(bodyId)) {
        b2Body_ApplyLinearImpulseToCenter(bodyId, {impulse.x, impulse.y}, wake);
    }
}

void RigidBody2DUtils::setLinearVelocity(b2BodyId bodyId, glm::vec2 velocity) {
    if (B2_IS_NON_NULL(bodyId)) {
        b2Body_SetLinearVelocity(bodyId, {velocity.x, velocity.y});
    }
}

glm::vec2 RigidBody2DUtils::getLinearVelocity(b2BodyId bodyId) {
    if (B2_IS_NON_NULL(bodyId)) {
        b2Vec2 v = b2Body_GetLinearVelocity(bodyId);
        return {v.x, v.y};
    }
    return {0.0f, 0.0f};
}

glm::vec2 RigidBody2DUtils::getPosition(b2BodyId bodyId) {
    if (B2_IS_NON_NULL(bodyId)) {
        b2Vec2 p = b2Body_GetPosition(bodyId);
        return {p.x, p.y};
    }
    return {0.0f, 0.0f};
}

} // namespace engine
