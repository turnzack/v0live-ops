#include <doctest/doctest.h>
#include "engine/ecs/World.hpp"
#include "engine/physics2d/PhysicsWorld2D.hpp"

using namespace engine;

TEST_CASE("Dynamic body falls under gravity") {
    World world;
    PhysicsWorld2D physics;
    CHECK(physics.initialize(glm::vec2(0.0f, -9.81f)));

    const Entity entity = world.createEntity("Box");
    auto& transform = world.add<TransformComponent>(entity);
    transform.position = {0.0f, 10.0f, 0.0f};

    world.add<RigidBody2DComponent>(entity);
    auto& collider = world.add<Collider2DComponent>(entity);
    collider.type = Collider2DType::Box;
    collider.size = {1.0f, 1.0f};

    physics.syncFromEcs(world);

    const float startY = transform.position.y;
    for (int i = 0; i < 60; ++i) {
        physics.step(1.0f / 60.0f, 4);
        physics.syncToEcs(world);
    }

    CHECK(transform.position.y < startY);
}
