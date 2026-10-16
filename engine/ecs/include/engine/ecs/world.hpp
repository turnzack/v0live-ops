#pragma once

#include <cstdint>

namespace engine::ecs {

using EntityId = std::uint64_t;

class World {
public:
    World() = default;
    ~World() = default;

    [[nodiscard]] EntityId createEntity() noexcept {
        return ++m_nextEntityId;
    }

    [[nodiscard]] std::size_t entityCount() const noexcept {
        return m_nextEntityId;
    }

private:
    EntityId m_nextEntityId{0};
};

} // namespace engine::ecs
