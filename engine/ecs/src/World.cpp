#include "engine/ecs/World.hpp"
#include <algorithm>

namespace engine::ecs {

World::World() = default;

World::~World() {
    clear();
}

Entity World::createEntity(std::string_view name, const core::GUID& guid) {
    EntityId id = m_nextEntityId++;
    m_entities.push_back(id);
    m_alive[id] = true;

    core::GUID actualGuid = guid.isValid() ? guid : core::GUID::generate();
    addComponent<IDComponent>(id, actualGuid);
    addComponent<TagComponent>(id, name);
    addComponent<TransformComponent>(id);
    addComponent<HierarchyComponent>(id);

    m_guidToEntity[actualGuid] = id;
    return Entity(id, this);
}

void World::destroyEntity(EntityId id) {
    auto it = m_alive.find(id);
    if (it != m_alive.end() && it->second) {
        it->second = false;
        if (auto* idComp = getComponent<IDComponent>(id)) {
            m_guidToEntity.erase(idComp->id);
        }
        auto entIt = std::find(m_entities.begin(), m_entities.end(), id);
        if (entIt != m_entities.end()) {
            m_entities.erase(entIt);
        }
    }
}

bool World::isAlive(EntityId id) const noexcept {
    auto it = m_alive.find(id);
    return it != m_alive.end() && it->second;
}

size_t World::entityCount() const noexcept {
    return m_entities.size();
}

Entity World::getEntity(EntityId id) {
    return isAlive(id) ? Entity(id, this) : Entity();
}

Entity World::getEntityByGUID(const core::GUID& guid) {
    auto it = m_guidToEntity.find(guid);
    if (it != m_guidToEntity.end()) {
        return getEntity(it->second);
    }
    return Entity();
}

void World::clear() {
    m_entities.clear();
    m_alive.clear();
    m_guidToEntity.clear();
    m_componentPools.clear();
    m_nextEntityId = 1;
}

core::GUID Entity::getGUID() const {
    if (isValid()) {
        if (const auto* idComp = getComponent<IDComponent>()) {
            return idComp->id;
        }
    }
    return core::GUID::Invalid;
}

std::string_view Entity::getName() const {
    if (isValid()) {
        if (const auto* tagComp = getComponent<TagComponent>()) {
            return tagComp->tag;
        }
    }
    return "Invalid";
}

} // namespace engine::ecs
