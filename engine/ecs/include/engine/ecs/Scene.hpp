#pragma once

#include "engine/ecs/World.hpp"
#include <string>

namespace engine::ecs {

class Scene {
public:
    explicit Scene(std::string_view name = "Untitled Scene");
    ~Scene();

    [[nodiscard]] std::string_view getName() const noexcept { return m_name; }
    void setName(std::string_view name) { m_name = name; }

    [[nodiscard]] const core::GUID& getGUID() const noexcept { return m_guid; }
    void setGUID(const core::GUID& guid) noexcept { m_guid = guid; }

    [[nodiscard]] World& getWorld() noexcept { return m_world; }
    [[nodiscard]] const World& getWorld() const noexcept { return m_world; }

    Entity createEntity(std::string_view name = "Entity", const core::GUID& guid = core::GUID::Invalid);
    void destroyEntity(Entity entity);

    void clear();

private:
    std::string m_name;
    core::GUID m_guid;
    World m_world;
};

} // namespace engine::ecs
