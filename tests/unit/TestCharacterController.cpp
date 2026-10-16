#include <doctest/doctest.h>
#include <SDL3/SDL.h>
#include "engine/ecs/World.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/physics2d/CharacterController2D.hpp"
#include "engine/physics2d/PhysicsWorld2D.hpp"

using namespace engine;

TEST_CASE("Character jumps when grounded") {
    World world;
    InputManager input;
    PhysicsWorld2D physics;
    CharacterController2D controller;

    CHECK(physics.initialize());

    const Entity player = world.createEntity("Player");
    auto& transform = world.add<TransformComponent>(player);
    transform.position = glm::vec3(0.0f, 1.0f, 0.0f);

    world.add<RigidBody2DComponent>(player);
    world.add<Collider2DComponent>(player);
    world.add<CharacterController2DComponent>(player);

    physics.syncFromEcs(world);

    SDL_Event jump{};
    jump.type = SDL_EVENT_KEY_DOWN;
    jump.key.key = SDLK_SPACE;

    input.beginFrame();
    input.handleEvent(jump);

    controller.update(world, input, physics, 1.0f / 60.0f);

    const auto* rigidBody = world.tryGet<RigidBody2DComponent>(player);
    CHECK(rigidBody != nullptr);
}
