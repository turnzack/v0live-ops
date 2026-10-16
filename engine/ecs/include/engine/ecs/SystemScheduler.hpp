#pragma once

#include "engine/ecs/World.hpp"
#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

namespace engine::ecs {

enum class SystemPhase {
    PreUpdate,
    FixedUpdate,
    Update,
    PostUpdate,
    PreRender,
    Render,
    PostRender
};

using SystemFunction = std::function<void(World& world, float deltaTime)>;

struct RegisteredSystem {
    std::string name;
    SystemFunction function;
};

class SystemScheduler {
public:
    SystemScheduler() = default;

    void registerSystem(SystemPhase phase, std::string_view name, SystemFunction system) {
        m_phases[phase].push_back({std::string(name), std::move(system)});
    }

    void runPhase(SystemPhase phase, World& world, float deltaTime) {
        auto it = m_phases.find(phase);
        if (it != m_phases.end()) {
            for (const auto& sys : it->second) {
                sys.function(world, deltaTime);
            }
        }
    }

    [[nodiscard]] size_t getSystemCount(SystemPhase phase) const {
        auto it = m_phases.find(phase);
        return (it != m_phases.end()) ? it->second.size() : 0;
    }

    void clear() {
        m_phases.clear();
    }

private:
    std::unordered_map<SystemPhase, std::vector<RegisteredSystem>> m_phases;
};

} // namespace engine::ecs
