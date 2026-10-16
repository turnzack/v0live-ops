#include <doctest/doctest.h>
#include "engine/scripting/LuaRuntime.hpp"

using namespace engine;

TEST_CASE("LuaRuntime executes a simple script") {
    World world;
    InputManager input;
    AudioEngine audio;
    LuaRuntime lua;

    CHECK(lua.initialize(world, input, audio));

    const auto entity = world.createEntity("Player");
    auto* transform = world.tryGet<TransformComponent>(entity);
    REQUIRE(transform != nullptr);

    lua.lua()["player_transform"] = transform;

    CHECK(lua.loadScript("tests/fixtures/simple_player.lua"));
    CHECK(transform->position.x == doctest::Approx(10.0f));
}
