#include <doctest/doctest.h>
#include <fstream>
#include <engine/core/EngineConfig.hpp>

using namespace engine;

TEST_CASE("EngineConfig loads defaults when file is missing") {
    const auto config = EngineConfig::loadOrDefault("does_not_exist.json");
    CHECK(config.window.width == 1280);
    CHECK(config.window.height == 720);
    CHECK(config.loop.fixedUpdateHz == 60);
}

TEST_CASE("EngineConfig round-trips through JSON") {
    EngineConfig original;
    original.window.title = "Test";
    original.window.width = 800;
    original.loop.fixedUpdateHz = 120;
    original.save("test_config.json");

    const auto loaded = EngineConfig::loadOrDefault("test_config.json");
    CHECK(loaded.window.title == "Test");
    CHECK(loaded.window.width == 800);
    CHECK(loaded.loop.fixedUpdateHz == 120);
}
