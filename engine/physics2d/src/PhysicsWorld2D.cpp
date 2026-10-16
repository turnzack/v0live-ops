#include "engine/physics2d/PhysicsWorld2D.hpp"
#include <spdlog/spdlog.h>
#include "engine/core/Assert.hpp"

namespace engine {

bool PhysicsWorld2D::initialize(glm::vec2 gravity) {
    if (initialized_) {
        return true;
    }

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {gravity.x, gravity.y};

    worldId_ = b2CreateWorld(&worldDef);
    if (B2_IS_NULL(worldId_)) {
        spdlog::error("Failed to create Box2D world.");
        return false;
    }

    initialized_ = true;
    spdlog::info("Box2D world initialized with gravity ({}, {}).", gravity.x, gravity.y);
    return true;
}

void PhysicsWorld2D::shutdown() {
    if (!initialized_) {
        return;
    }

    b2DestroyWorld(worldId_);
    worldId_ = b2_nullWorldId;
    bodies_.clear();
    initialized_ = false;
}

void PhysicsWorld2D::setGravity(glm::vec2 gravity) {
    if (initialized_) {
        b2World_SetGravity(worldId_, {gravity.x, gravity.y});
    }
}

void PhysicsWorld2D::syncFromEcs(World& world) {
    if (!initialized_) {
        return;
    }

    world.forEach<
        TransformComponent,
        RigidBody2DComponent,
        Collider2DComponent
    >(
        [&](Entity entity, TransformComponent& transform, RigidBody2DComponent& rigidBody, Collider2DComponent& collider) {
            if (bodies_.contains(entity)) {
                return;
            }

            b2BodyDef bodyDef = b2DefaultBodyDef();
            switch (rigidBody.type) {
                case RigidBody2DType::Static:
                    bodyDef.type = b2_staticBody;
                    break;
                case RigidBody2DType::Kinematic:
                    bodyDef.type = b2_kinematicBody;
                    break;
                default:
                    bodyDef.type = b2_dynamicBody;
                    break;
            }

            bodyDef.position = {transform.position.x, transform.position.y};
            bodyDef.gravityScale = rigidBody.gravityScale;
            bodyDef.linearDamping = rigidBody.linearDamping;
            bodyDef.fixedRotation = rigidBody.fixedRotation;

            const b2BodyId bodyId = b2CreateBody(worldId_, &bodyDef);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = collider.density;
            shapeDef.friction = collider.friction;
            shapeDef.restitution = collider.restitution;
            shapeDef.isSensor = collider.isTrigger;
            shapeDef.enableContactEvents = collider.enableContactEvents;
            shapeDef.enableSensorEvents = collider.enableSensorEvents;
            shapeDef.filter.categoryBits = collider.categoryBits;
            shapeDef.filter.maskBits = collider.maskBits;

            switch (collider.type) {
                case Collider2DType::Circle: {
                    const b2Circle circle{
                        {0.0f, 0.0f},
                        collider.radius
                    };
                    b2CreateCircleShape(bodyId, &shapeDef, &circle);
                    break;
                }
                case Collider2DType::Capsule: {
                    const b2Capsule capsule{
                        {0.0f, -collider.size.y * 0.5f},
                        {0.0f, collider.size.y * 0.5f},
                        collider.radius
                    };
                    b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);
                    break;
                }
                default: {
                    const b2Polygon box = b2MakeOffsetBox(
                        collider.size.x * 0.5f,
                        collider.size.y * 0.5f,
                        {0.0f, 0.0f},
                        b2MakeRot(0.0f)
                    );
                    b2CreatePolygonShape(bodyId, &shapeDef, &box);
                    break;
                }
            }

            bodies_[entity] = BodyRecord{
                .bodyId = bodyId,
                .entity = entity
            };
        }
    );
}

void PhysicsWorld2D::step(float fixedDelta, int subStepCount) {
    if (!initialized_) {
        return;
    }
    b2World_Step(worldId_, fixedDelta, subStepCount);
}

void PhysicsWorld2D::syncToEcs(World& world) {
    if (!initialized_) {
        return;
    }

    const b2BodyEvents bodyEvents = b2World_GetBodyEvents(worldId_);
    for (int i = 0; i < bodyEvents.moveCount; ++i) {
        const b2BodyMoveEvent& event = bodyEvents.moveEvents[i];
        for (const auto& [entity, record] : bodies_) {
            if (record.bodyId.index1 != event.bodyId.index1 || record.bodyId.world0 != event.bodyId.world0) {
                continue;
            }
            if (auto* transform = world.tryGet<TransformComponent>(entity)) {
                transform->position.x = event.transform.p.x;
                transform->position.y = event.transform.p.y;
                transform->rotation.z = b2Rot_GetAngle(event.transform.q);
            }
            break;
        }
    }
}

bool PhysicsWorld2D::raycast(
    glm::vec2 origin,
    glm::vec2 direction,
    float maxDistance
) const {
    if (!initialized_) {
        return false;
    }

    const b2QueryFilter filter = b2DefaultQueryFilter();
    const b2RayResult result = b2World_CastRayClosest(
        worldId_,
        {origin.x, origin.y},
        {direction.x * maxDistance, direction.y * maxDistance},
        filter
    );

    return B2_IS_NON_NULL(result.shapeId);
}

b2BodyId PhysicsWorld2D::bodyIdFor(Entity entity) const {
    const auto it = bodies_.find(entity);
    if (it == bodies_.end()) {
        return b2_nullBodyId;
    }
    return it->second.bodyId;
}

} // namespace engine
