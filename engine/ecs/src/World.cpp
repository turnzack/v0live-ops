#include "engine/ecs/World.hpp"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

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

Entity World::getEntityByName(std::string_view name) {
    for (EntityId id : m_entities) {
        if (isAlive(id)) {
            if (const auto* tag = getComponent<TagComponent>(id)) {
                if (tag->tag == name || tag->name == name) {
                    return Entity(id, this);
                }
            }
        }
    }
    return Entity();
}

bool World::isValid(const Entity& entity) const noexcept {
    return entity.isValid();
}

bool World::saveScene(std::string_view filePath) {
    nlohmann::json root;
    root["version"] = 1;
    root["entities"] = nlohmann::json::array();

    for (EntityId id : m_entities) {
        if (!isAlive(id)) continue;

        nlohmann::json entityJson;
        if (const auto* idComp = getComponent<IDComponent>(id)) {
            entityJson["id"] = idComp->id.asString();
        }
        if (const auto* tag = getComponent<TagComponent>(id)) {
            entityJson["tag"] = tag->tag;
        }
        if (const auto* transform = getComponent<TransformComponent>(id)) {
            entityJson["transform"] = {
                {"position", {{"x", transform->position.x}, {"y", transform->position.y}, {"z", transform->position.z}}},
                {"rotation", {{"x", transform->rotation.x}, {"y", transform->rotation.y}, {"z", transform->rotation.z}}},
                {"scale", {{"x", transform->scale.x}, {"y", transform->scale.y}, {"z", transform->scale.z}}}
            };
        }
        if (const auto* sprite = getComponent<SpriteComponent>(id)) {
            entityJson["sprite"] = {
                {"size", {{"x", sprite->size.x}, {"y", sprite->size.y}}},
                {"colorTint", {{"x", sprite->colorTint.x}, {"y", sprite->colorTint.y}, {"z", sprite->colorTint.z}, {"w", sprite->colorTint.w}}},
                {"sortingLayer", sprite->sortingLayer},
                {"flipX", sprite->flipX},
                {"flipY", sprite->flipY}
            };
        }
        if (const auto* movement = getComponent<MovementComponent>(id)) {
            entityJson["movement"] = {
                {"speed", movement->speed},
                {"velocity", {{"x", movement->velocity.x}, {"y", movement->velocity.y}}}
            };
        }
        root["entities"].push_back(entityJson);
    }

    std::ofstream file(std::string(filePath));
    if (!file) return false;
    file << root.dump(2) << '\n';
    return true;
}

bool World::loadScene(std::string_view filePath) {
    std::ifstream file(std::string(filePath));
    if (!file) return false;

    nlohmann::json root;
    try {
        file >> root;
    } catch (...) {
        return false;
    }

    clear();

    if (root.contains("entities") && root["entities"].is_array()) {
        for (const auto& entityJson : root["entities"]) {
            std::string name = entityJson.value("tag", "Entity");
            std::string idStr = entityJson.value("id", "");
            core::GUID guid(idStr);

            Entity ent = createEntity(name, guid);
            if (entityJson.contains("transform")) {
                const auto& t = entityJson["transform"];
                auto* comp = ent.getComponent<TransformComponent>();
                if (comp) {
                    if (t.contains("position")) {
                        comp->position = {t["position"].value("x", 0.0f), t["position"].value("y", 0.0f), t["position"].value("z", 0.0f)};
                    }
                    if (t.contains("rotation")) {
                        comp->rotation = {t["rotation"].value("x", 0.0f), t["rotation"].value("y", 0.0f), t["rotation"].value("z", 0.0f)};
                    }
                    if (t.contains("scale")) {
                        comp->scale = {t["scale"].value("x", 1.0f), t["scale"].value("y", 1.0f), t["scale"].value("z", 1.0f)};
                    }
                }
            }
            if (entityJson.contains("sprite")) {
                const auto& s = entityJson["sprite"];
                core::Vec2 sz{s["size"].value("x", 32.0f), s["size"].value("y", 32.0f)};
                core::Vec4 col{s["colorTint"].value("x", 1.0f), s["colorTint"].value("y", 1.0f), s["colorTint"].value("z", 1.0f), s["colorTint"].value("w", 1.0f)};
                ent.addComponent<SpriteComponent>(sz, col);
            }
            if (entityJson.contains("movement")) {
                const auto& m = entityJson["movement"];
                float spd = m.value("speed", 200.0f);
                auto& mov = ent.addComponent<MovementComponent>(spd);
                if (m.contains("velocity")) {
                    mov.velocity = {m["velocity"].value("x", 0.0f), m["velocity"].value("y", 0.0f)};
                }
            }
        }
    }
    return true;
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
