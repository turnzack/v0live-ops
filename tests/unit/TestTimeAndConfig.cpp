#include "engine/core/Config.hpp"
#include "engine/core/Time.hpp"
#include <iostream>
#include <string_view>
#include <functional>
#include <thread>
#include <chrono>

namespace test {
    void registerTest(std::string_view name, std::function<bool()> testFunc);
}

#define TEST_CASE(name) \
    static bool test_func_##name(); \
    static const bool test_registered_##name = []() { \
        ::test::registerTest(#name, test_func_##name); \
        return true; \
    }(); \
    static bool test_func_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (false)

using namespace engine::core;

TEST_CASE(Config_DefaultsAndSerialization) {
    EngineConfig cfg{};
    CHECK(cfg.window.width == 1280);
    CHECK(cfg.window.height == 720);
    CHECK(cfg.window.title == "Engine Suite");
    CHECK(cfg.simulation.fixedTimestepHz == 60.0f);
    CHECK(std::abs(cfg.getFixedDeltaTime() - (1.0f / 60.0f)) < 1e-5f);

    const std::string json = cfg.toJsonString();
    CHECK(json.find("\"title\": \"Engine Suite\"") != std::string::npos);
    CHECK(json.find("\"width\": 1280") != std::string::npos);

    EngineConfig reloaded = EngineConfig::fromJsonString(json);
    CHECK(reloaded.window.width == 1280);
    CHECK(reloaded.window.height == 720);
    CHECK(reloaded.window.title == "Engine Suite");
    CHECK(reloaded.window.vsync == true);

    return true;
}

TEST_CASE(Config_CustomParsing) {
    const std::string customJson = R"({
        "window": {
            "title": "Custom Game",
            "width": 1920,
            "height": 1080,
            "vsync": false,
            "resizable": true
        },
        "simulation": {
            "fixedTimestepHz": 120.0
        }
    })";

    EngineConfig cfg = EngineConfig::fromJsonString(customJson);
    CHECK(cfg.window.title == "Custom Game");
    CHECK(cfg.window.width == 1920);
    CHECK(cfg.window.height == 1080);
    CHECK(cfg.window.vsync == false);
    CHECK(cfg.simulation.fixedTimestepHz == 120.0f);
    CHECK(std::abs(cfg.getFixedDeltaTime() - (1.0f / 120.0f)) < 1e-5f);

    return true;
}

TEST_CASE(Time_FixedTimestepAndAlpha) {
    Time time(60.0f, 0.2f);
    CHECK(std::abs(time.getFixedDeltaTime() - (1.0f / 60.0f)) < 1e-5f);
    CHECK(!time.shouldDoFixedStep());

    // Simulation d'un saut de temps de 35ms (doit donner 2 pas fixes à 60Hz: 16.6ms * 2 = 33.3ms)
    std::this_thread::sleep_for(std::chrono::milliseconds(35));
    time.tick();

    CHECK(time.getDeltaTime() >= 0.030f);
    CHECK(time.shouldDoFixedStep()); // 1er pas consommé
    CHECK(time.shouldDoFixedStep()); // 2ème pas consommé
    CHECK(!time.shouldDoFixedStep()); // Plus de pas complet

    CHECK(time.getAlpha() >= 0.0f && time.getAlpha() <= 1.0f);

    return true;
}
