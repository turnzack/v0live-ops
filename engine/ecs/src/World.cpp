#if defined(_MSC_VER)
#include <intrin.h>
#endif

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
    root["format"] = "engine.scene";
    root["version"] = 1;
    root["entities"] = nlohmann::json::array();

    for (EntityId id : m_entities) {
        if (!isAlive(id)) continue;

        nlohmann::json entityJson;
        if (const auto* idComp = getComponent<IDComponent>(id)) {
            entityJson["id"] = idComp->id.asString();
            entityJson["guid"] = idComp->id.asString();
        }
        if (const auto* tag = getComponent<TagComponent>(id)) {
            entityJson["tag"] = tag->tag;
            entityJson["name"] = tag->name;
        }

        nlohmann::json compsJson;

        if (const auto* transform = getComponent<TransformComponent>(id)) {
            entityJson["transform"] = {
                {"position", {{"x", transform->position.x}, {"y", transform->position.y}, {"z", transform->position.z}}},
                {"rotation", {{"x", transform->rotation.x}, {"y", transform->rotation.y}, {"z", transform->rotation.z}}},
                {"scale", {{"x", transform->scale.x}, {"y", transform->scale.y}, {"z", transform->scale.z}}}
            };
            compsJson["Transform"] = {
                {"position", {transform->position.x, transform->position.y, transform->position.z}},
                {"rotation", {transform->rotation.x, transform->rotation.y, transform->rotation.z}},
                {"scale", {transform->scale.x, transform->scale.y, transform->scale.z}}
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
            compsJson["SpriteComponent"] = {
                {"size", {sprite->size.x, sprite->size.y}},
                {"sortingLayer", sprite->sortingLayer}
            };
        }
        if (const auto* movement = getComponent<MovementComponent>(id)) {
            entityJson["movement"] = {
                {"speed", movement->speed},
                {"velocity", {{"x", movement->velocity.x}, {"y", movement->velocity.y}}}
            };
        }
        if (const auto* rb = getComponent<RigidBody2DComponent>(id)) {
            std::string typeStr = (rb->type == RigidBody2DType::Static) ? "Static" :
                                  (rb->type == RigidBody2DType::Kinematic) ? "Kinematic" : "Dynamic";
            compsJson["RigidBody2DComponent"] = {
                {"type", typeStr},
                {"gravityScale", rb->gravityScale},
                {"linearDamping", rb->linearDamping},
                {"fixedRotation", rb->fixedRotation},
                {"isSensor", rb->isSensor}
            };
        }
        if (const auto* col = getComponent<Collider2DComponent>(id)) {
            std::string typeStr = (col->type == Collider2DType::Circle) ? "Circle" :
                                  (col->type == Collider2DType::Capsule) ? "Capsule" : "Box";
            compsJson["Collider2DComponent"] = {
                {"type", typeStr},
                {"size", {col->size.x, col->size.y}},
                {"radius", col->radius},
                {"density", col->density},
                {"friction", col->friction},
                {"restitution", col->restitution},
                {"isTrigger", col->isTrigger}
            };
        }
        if (const auto* cc = getComponent<CharacterController2DComponent>(id)) {
            compsJson["CharacterController2DComponent"] = {
                {"maxSpeed", cc->maxSpeed},
                {"acceleration", cc->acceleration},
                {"jumpVelocity", cc->jumpVelocity},
                {"coyoteTime", cc->coyoteTime},
                {"jumpBufferTime", cc->jumpBufferTime}
            };
        }

        entityJson["components"] = compsJson;
        root["entities"].push_back(entityJson);
    }

    std::string outPath(filePath);
    std::ofstream file(outPath);
    if (!file) return false;
    file << root.dump(2) << '\n';
    return true;
}

bool World::loadScene(std::string_view filePath) {
    std::string inPath(filePath);
    std::ifstream file(inPath);
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
            std::string name = entityJson.value("name", entityJson.value("tag", "Entity"));
            std::string idStr = entityJson.value("guid", entityJson.value("id", ""));
            core::GUID guid(idStr);

            Entity ent = createEntity(name, guid);

            // 1. Direct transform or under components
            const bool hasComps = entityJson.contains("components") && entityJson["components"].is_object();
            const auto& comps = hasComps ? entityJson["components"] : entityJson;

            if (comps.contains("Transform") || comps.contains("transform")) {
                const auto& t = comps.contains("Transform") ? comps["Transform"] : comps["transform"];
                auto* comp = ent.getComponent<TransformComponent>();
                if (comp) {
                    if (t.contains("position")) {
                        if (t["position"].is_array() && t["position"].size() >= 3) {
                            comp->position = {t["position"][0].get<float>(), t["position"][1].get<float>(), t["position"][2].get<float>()};
                        } else if (t["position"].is_object()) {
                            comp->position = {t["position"].value("x", 0.0f), t["position"].value("y", 0.0f), t["position"].value("z", 0.0f)};
                        }
                    }
                    if (t.contains("rotation")) {
                        if (t["rotation"].is_array() && t["rotation"].size() >= 3) {
                            comp->rotation = {t["rotation"][0].get<float>(), t["rotation"][1].get<float>(), t["rotation"][2].get<float>()};
                        } else if (t["rotation"].is_object()) {
                            comp->rotation = {t["rotation"].value("x", 0.0f), t["rotation"].value("y", 0.0f), t["rotation"].value("z", 0.0f)};
                        }
                    }
                    if (t.contains("scale")) {
                        if (t["scale"].is_array() && t["scale"].size() >= 3) {
                            comp->scale = {t["scale"][0].get<float>(), t["scale"][1].get<float>(), t["scale"][2].get<float>()};
                        } else if (t["scale"].is_object()) {
                            comp->scale = {t["scale"].value("x", 1.0f), t["scale"].value("y", 1.0f), t["scale"].value("z", 1.0f)};
                        }
                    }
                }
            }

            if (comps.contains("SpriteComponent") || comps.contains("sprite")) {
                const auto& s = comps.contains("SpriteComponent") ? comps["SpriteComponent"] : comps["sprite"];
                core::Vec2 sz{32.0f, 32.0f};
                if (s.contains("size")) {
                    if (s["size"].is_array() && s["size"].size() >= 2) {
                        sz = {s["size"][0].get<float>(), s["size"][1].get<float>()};
                    } else if (s["size"].is_object()) {
                        sz = {s["size"].value("x", 32.0f), s["size"].value("y", 32.0f)};
                    }
                }
                auto& spr = ent.addComponent<SpriteComponent>(sz);
                spr.sortingLayer = s.value("sortingLayer", 0);
            }

            if (comps.contains("MovementComponent") || comps.contains("movement")) {
                const auto& m = comps.contains("MovementComponent") ? comps["MovementComponent"] : comps["movement"];
                float spd = m.value("speed", 200.0f);
                auto& mov = ent.addComponent<MovementComponent>(spd);
                if (m.contains("velocity") && m["velocity"].is_object()) {
                    mov.velocity = {m["velocity"].value("x", 0.0f), m["velocity"].value("y", 0.0f)};
                }
            }

            if (comps.contains("RigidBody2DComponent")) {
                const auto& r = comps["RigidBody2DComponent"];
                auto& rb = ent.addComponent<RigidBody2DComponent>();
                std::string tStr = r.value("type", "Dynamic");
                if (tStr == "Static") rb.type = RigidBody2DType::Static;
                else if (tStr == "Kinematic") rb.type = RigidBody2DType::Kinematic;
                else rb.type = RigidBody2DType::Dynamic;

                rb.gravityScale = r.value("gravityScale", 1.0f);
                rb.linearDamping = r.value("linearDamping", 0.0f);
                rb.fixedRotation = r.value("fixedRotation", false);
                rb.isSensor = r.value("isSensor", false);
            }

            if (comps.contains("Collider2DComponent")) {
                const auto& c = comps["Collider2DComponent"];
                auto& col = ent.addComponent<Collider2DComponent>();
                std::string tStr = c.value("type", "Box");
                if (tStr == "Circle") col.type = Collider2DType::Circle;
                else if (tStr == "Capsule") col.type = Collider2DType::Capsule;
                else col.type = Collider2DType::Box;

                if (c.contains("size")) {
                    if (c["size"].is_array() && c["size"].size() >= 2) {
                        col.size = {c["size"][0].get<float>(), c["size"][1].get<float>()};
                    } else if (c["size"].is_object()) {
                        col.size = {c["size"].value("x", 1.0f), c["size"].value("y", 1.0f)};
                    }
                }
                col.radius = c.value("radius", 0.5f);
                col.density = c.value("density", 1.0f);
                col.friction = c.value("friction", 0.3f);
                col.restitution = c.value("restitution", 0.0f);
                col.isTrigger = c.value("isTrigger", false);
                col.enableContactEvents = c.value("enableContactEvents", false);
                col.enableSensorEvents = c.value("enableSensorEvents", false);
            }

            if (comps.contains("CharacterController2DComponent")) {
                const auto& cc = comps["CharacterController2DComponent"];
                auto& ctrl = ent.addComponent<CharacterController2DComponent>();
                ctrl.maxSpeed = cc.value("maxSpeed", 350.0f);
                ctrl.acceleration = cc.value("acceleration", 2500.0f);
                ctrl.jumpVelocity = cc.value("jumpVelocity", 750.0f);
                ctrl.coyoteTime = cc.value("coyoteTime", 0.12f);
                ctrl.jumpBufferTime = cc.value("jumpBufferTime", 0.12f);
                ctrl.maxSlopeAngle = cc.value("maxSlopeAngle", 45.0f);
            }
        }
    }
    return true;
}

World World::clone() const {
    World copy;
    copy.m_nextEntityId = m_nextEntityId;
    copy.m_entities = m_entities;
    copy.m_alive = m_alive;
    copy.m_guidToEntity = m_guidToEntity;
    copy.m_componentPools = m_componentPools;
    return copy;
}

void World::restoreFrom(const World& source) {
    m_nextEntityId = source.m_nextEntityId;
    m_entities = source.m_entities;
    m_alive = source.m_alive;
    m_guidToEntity = source.m_guidToEntity;
    m_componentPools = source.m_componentPools;
}

void World::setParent(Entity child, Entity parent) {
    if (child.isValid() && parent.isValid()) {
        setParent(child.getId(), parent.getId());
    }
}

void World::clearParent(Entity child) {
    if (child.isValid()) {
        detachFromParent(child.getId());
    }
}

std::vector<Entity> World::children(Entity parent) const {
    if (!parent.isValid()) return {};
    std::vector<Entity> res;
    auto childIds = getChildren(parent.getId());
    res.reserve(childIds.size());
    for (EntityId id : childIds) {
        res.push_back(const_cast<World*>(this)->getEntity(id));
    }
    return res;
}

std::optional<Entity> World::parent(Entity child) const {
    if (!child.isValid()) return std::nullopt;
    EntityId pId = getParent(child.getId());
    if (pId != kInvalidEntityId && isAlive(pId)) {
        return const_cast<World*>(this)->getEntity(pId);
    }
    return std::nullopt;
}

bool World::isDescendantOf(EntityId entityId, EntityId potentialAncestorId) const {
    if (entityId == kInvalidEntityId || potentialAncestorId == kInvalidEntityId) {
        return false;
    }
    EntityId current = getParent(entityId);
    while (current != kInvalidEntityId) {
        if (current == potentialAncestorId) {
            return true;
        }
        current = getParent(current);
    }
    return false;
}

EntityId World::getParent(EntityId entityId) const {
    if (const auto* hier = getComponent<HierarchyComponent>(entityId)) {
        return hier->parent;
    }
    return kInvalidEntityId;
}

std::vector<EntityId> World::getChildren(EntityId entityId) const {
    if (const auto* hier = getComponent<HierarchyComponent>(entityId)) {
        return hier->children;
    }
    return {};
}

std::vector<EntityId> World::getRootEntities() const {
    std::vector<EntityId> roots;
    for (EntityId id : m_entities) {
        if (isAlive(id)) {
            if (const auto* hier = getComponent<HierarchyComponent>(id)) {
                if (hier->parent == kInvalidEntityId || !isAlive(hier->parent)) {
                    roots.push_back(id);
                }
            } else {
                roots.push_back(id);
            }
        }
    }
    return roots;
}

void World::detachFromParent(EntityId childId) {
    if (!isAlive(childId)) return;

    auto* childHier = getComponent<HierarchyComponent>(childId);
    if (!childHier || childHier->parent == kInvalidEntityId) return;

    EntityId oldParentId = childHier->parent;
    childHier->parent = kInvalidEntityId;

    if (auto* parentHier = getComponent<HierarchyComponent>(oldParentId)) {
        auto it = std::find(parentHier->children.begin(), parentHier->children.end(), childId);
        if (it != parentHier->children.end()) {
            parentHier->children.erase(it);
        }
    }
}

bool World::setParent(EntityId childId, EntityId newParentId) {
    if (!isAlive(childId) || childId == newParentId) {
        return false;
    }

    if (newParentId == kInvalidEntityId) {
        detachFromParent(childId);
        return true;
    }

    if (!isAlive(newParentId)) {
        return false;
    }

    // Circular dependency check: newParent cannot be a descendant of child
    if (isDescendantOf(newParentId, childId)) {
        return false;
    }

    detachFromParent(childId);

    auto* childHier = getComponent<HierarchyComponent>(childId);
    if (!childHier) {
        childHier = &addComponent<HierarchyComponent>(childId);
    }
    childHier->parent = newParentId;

    auto* parentHier = getComponent<HierarchyComponent>(newParentId);
    if (!parentHier) {
        parentHier = &addComponent<HierarchyComponent>(newParentId);
    }
    parentHier->children.push_back(childId);

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
