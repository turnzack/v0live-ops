#include "engine/core/EngineConfig.hpp"
#include <fstream>
#include <spdlog/spdlog.h>
#include "engine/core/Assert.hpp"

namespace engine {

EngineConfig EngineConfig::loadOrDefault(const std::string& path) {
    EngineConfig config;
    std::ifstream file(path);
    if (!file) {
        spdlog::warn("Config file not found: {}. Using defaults.", path);
        return config;
    }
    try {
        nlohmann::json json;
        file >> json;
        config = json.get<EngineConfig>();
        if (config.loop.fixedUpdateHz <= 0) {
            ENGINE_VERIFY(false, "fixedUpdateHz must be greater than zero");
        }
        if (config.loop.maxFixedStepsPerFrame <= 0) {
            ENGINE_VERIFY(false, "maxFixedStepsPerFrame must be greater than zero");
        }
    } catch (const std::exception& error) {
        spdlog::error("Failed to load config {}: {}", path, error.what());
        config = EngineConfig{};
    }
    return config;
}

void EngineConfig::save(const std::string& path) const {
    nlohmann::json json = *this;
    std::ofstream file(path);
    if (!file) {
        spdlog::error("Failed to write config: {}", path);
        return;
    }
    file << json.dump(4) << '\n';
}

} // namespace engine
