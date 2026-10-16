#pragma once

#include <string>
#include <string_view>
#include <cstdint>

namespace engine::core {

struct WindowConfig {
    std::string title{"Engine Suite"};
    uint32_t width{1280};
    uint32_t height{720};
    bool vsync{true};
    bool resizable{true};
    bool fullscreen{false};
};

struct SimulationConfig {
    float fixedTimestepHz{60.0f};
    float maxAccumulatedSeconds{0.2f}; // Évite le 'spiral of death'
};

struct EngineConfig {
    WindowConfig window{};
    SimulationConfig simulation{};

    [[nodiscard]] float getFixedDeltaTime() const noexcept {
        return (simulation.fixedTimestepHz > 0.0f) ? (1.0f / simulation.fixedTimestepHz) : (1.0f / 60.0f);
    }

    static EngineConfig loadFromFile(std::string_view filePath);
    bool saveToFile(std::string_view filePath) const;
    [[nodiscard]] std::string toJsonString() const;
    static EngineConfig fromJsonString(std::string_view jsonContent);
};

} // namespace engine::core
