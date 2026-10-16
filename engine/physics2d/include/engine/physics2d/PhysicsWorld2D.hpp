#pragma once

#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "engine/ecs/World.hpp"

namespace engine {

class PhysicsWorld2D {
public:
    PhysicsWorld2D() = default;
    ~PhysicsWorld2D() { shutdown(); }

    bool initialize(glm::vec2 gravity = glm::vec2(0.0f, -9.81f));
    void shutdown();

    void syncFromEcs(World& world);
    void step(float fixedDelta, int subStepCount = 4);
    void syncToEcs(World& world);

    [[nodiscard]] bool raycast(
        glm::vec2 origin,
        glm::vec2 direction,
        float maxDistance
    ) const;

    [[nodiscard]] b2BodyId bodyIdFor(Entity entity) const;
    [[nodiscard]] b2WorldId worldId() const noexcept { return worldId_; }
    [[nodiscard]] bool isInitialized() const noexcept { return initialized_; }

    void setGravity(glm::vec2 gravity);

private:
    struct BodyRecord {
        b2BodyId bodyId{};
        Entity entity{};
    };

    b2WorldId worldId_{};
    bool initialized_ = false;
    std::unordered_map<Entity, BodyRecord> bodies_;
};

} // namespace engine
