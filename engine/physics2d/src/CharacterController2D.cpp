#include "engine/physics2d/CharacterController2D.hpp"
#include <box2d/box2d.h>
#include <algorithm>
#include <cmath>

namespace engine {

void CharacterController2D::update(
    World& world,
    InputManager& input,
    PhysicsWorld2D& physics,
    float fixedDelta
) {
    world.forEach<
        TransformComponent,
        RigidBody2DComponent,
        Collider2DComponent,
        CharacterController2DComponent
    >(
        [&](Entity entity, TransformComponent& transform, RigidBody2DComponent& rigidBody, Collider2DComponent& collider, CharacterController2DComponent& controller) {
            if (rigidBody.type != RigidBody2DType::Dynamic) {
                return;
            }

            updateTimers(controller, fixedDelta);

            const glm::vec2 axis = input.movementAxis();
            const float targetVelocityX = axis.x * controller.maxSpeed;

            const b2BodyId bodyId = physics.bodyIdFor(entity);
            if (B2_IS_NULL(bodyId)) {
                return;
            }

            const b2Vec2 currentVelocity = b2Body_GetLinearVelocity(bodyId);
            const float t = std::clamp(controller.acceleration * fixedDelta / std::max(controller.maxSpeed, 1.0f), 0.0f, 1.0f);
            const float newVelocityX = std::lerp(currentVelocity.x, targetVelocityX, t);

            b2Body_SetLinearVelocity(bodyId, {newVelocityX, currentVelocity.y});

            if (input.wasPressed(InputAction::Jump)) {
                controller.jumpBufferTimer = controller.jumpBufferTime;
            }

            const bool canJump = (controller.grounded || controller.coyoteTimer > 0.0f);

            if (controller.jumpBufferTimer > 0.0f && canJump) {
                b2Body_SetLinearVelocity(bodyId, {newVelocityX, controller.jumpVelocity});
                controller.jumpBufferTimer = 0.0f;
                controller.coyoteTimer = 0.0f;
                controller.grounded = false;
                controller.jumping = true;
            }

            const glm::vec2 feetPosition{
                transform.position.x,
                transform.position.y - collider.size.y * 0.5f - 0.05f
            };

            controller.grounded = physics.raycast(
                feetPosition,
                glm::vec2(0.0f, -1.0f),
                0.15f
            );

            if (controller.grounded) {
                controller.coyoteTimer = controller.coyoteTime;
                controller.jumping = false;
            }
        }
    );
}

void CharacterController2D::updateTimers(
    CharacterController2DComponent& controller,
    float fixedDelta
) {
    controller.coyoteTimer = std::max(0.0f, controller.coyoteTimer - fixedDelta);
    controller.jumpBufferTimer = std::max(0.0f, controller.jumpBufferTimer - fixedDelta);
}

} // namespace engine
