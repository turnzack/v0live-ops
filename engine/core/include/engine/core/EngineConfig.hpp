#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace engine {

struct WindowConfig {
    std::string title = "Engine Suite";
    int width = 1280;
    int height = 720;
    bool resizable = true;
    bool vsync = true;
};

struct LoopConfig {
    int fixedUpdateHz = 60;
    int maxFixedStepsPerFrame = 5;
    double maxFrameDelta = 0.25;
};

struct EngineConfig {
    WindowConfig window;
    LoopConfig loop;

    static EngineConfig loadOrDefault(const std::string& path);
    void save(const std::string& path) const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WindowConfig, title, width, height, resizable, vsync)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoopConfig, fixedUpdateHz, maxFixedStepsPerFrame, maxFrameDelta)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EngineConfig, window, loop)

} // namespace engine
