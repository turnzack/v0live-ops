#pragma once

#include "engine/ecs/Components.hpp"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <any>
#include <functional>
#include <vector>
#include <optional>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace engine::ecs {

class Entity;

class World {
public:
    World();
    ~World();

    Entity createEntity(std::string_view name = "Entity", const core::GUID& guid = core::GUID::Invalid);
    void destroyEntity(EntityId id);

    [[nodiscard]] bool isAlive(EntityId id) const noexcept;
    [[nodiscard]] size_t entityCount() const noexcept;

    [[nodiscard]] Entity getEntity(EntityId id);
    [[nodiscard]] Entity getEntityByGUID(const core::GUID& guid);

    template <typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args) {
        auto& pool = getOrCreateComponentPool<T>();
        pool[id] = T(std::forward<Args>(args)...);
        return pool[id];
    }

    template <typename T>
    [[nodiscard]] T* getComponent(EntityId id) {
        auto it = m_componentPools.find(typeid(T));
        if (it == m_componentPools.end()) return nullptr;
        auto& pool = std::any_cast<std::unordered_map<EntityId, T>&>(it->second);
        auto elem = pool.find(id);
        return (elem != pool.end()) ? &elem->second : nullptr;
    }

    template <typename T>
    [[nodiscard]] const T* getComponent(EntityId id) const {
        auto it = m_componentPools.find(typeid(T));
        if (it == m_componentPools.end()) return nullptr;
        const auto& pool = std::any_cast<const std::unordered_map<EntityId, T>&>(it->second);
        auto elem = pool.find(id);
        return (elem != pool.end()) ? &elem->second : nullptr;
    }

    template <typename T>
    [[nodiscard]] bool hasComponent(EntityId id) const noexcept {
        return getComponent<T>(id) != nullptr;
    }

    template <typename T>
    void removeComponent(EntityId id) {
        auto it = m_componentPools.find(typeid(T));
        if (it != m_componentPools.end()) {
            auto& pool = std::any_cast<std::unordered_map<EntityId, T>&>(it->second);
            pool.erase(id);
        }
    }

    template <typename T, typename... Args>
    T& add(EntityId id, Args&&... args) {
        return addComponent<T>(id, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    T& add(const Entity& entity, Args&&... args);

    template <typename T1, typename... Rest, typename Func>
    void forEach(Func&& func) {
        auto it = m_componentPools.find(typeid(T1));
        if (it == m_componentPools.end()) return;
        auto& pool = std::any_cast<std::unordered_map<EntityId, T1>&>(it->second);
        for (auto& [entityId, first] : pool) {
            if (!isAlive(entityId)) continue;
            if constexpr (sizeof...(Rest) == 0) {
                if constexpr (std::is_invocable_v<Func, Entity, T1&>) {
                    func(getEntity(entityId), first);
                } else if constexpr (std::is_invocable_v<Func, EntityId, T1&>) {
                    func(entityId, first);
                } else if constexpr (std::is_invocable_v<Func, T1&>) {
                    func(first);
                }
            } else {
                if ((hasComponent<Rest>(entityId) && ...)) {
                    if constexpr (std::is_invocable_v<Func, Entity, T1&, Rest&...>) {
                        func(getEntity(entityId), first, *getComponent<Rest>(entityId)...);
                    } else if constexpr (std::is_invocable_v<Func, EntityId, T1&, Rest&...>) {
                        func(entityId, first, *getComponent<Rest>(entityId)...);
                    } else if constexpr (std::is_invocable_v<Func, T1&, Rest&...>) {
                        func(first, *getComponent<Rest>(entityId)...);
                    }
                }
            }
        }
    }

    [[nodiscard]] const std::vector<EntityId>& getAllEntities() const noexcept { return m_entities; }

    [[nodiscard]] Entity getEntityByName(std::string_view name);
    [[nodiscard]] bool isValid(const Entity& entity) const noexcept;

    template <typename T>
    [[nodiscard]] T* tryGet(const Entity& entity);

    template <typename T>
    [[nodiscard]] const T* tryGet(const Entity& entity) const;

    bool saveScene(std::string_view filePath);
    bool loadScene(std::string_view filePath);

    [[nodiscard]] World clone() const;
    void restoreFrom(const World& source);

    // Entity Hierarchy (EDT-020)
    void setParent(Entity child, Entity parent);
    void clearParent(Entity child);
    [[nodiscard]] std::vector<Entity> children(Entity parent) const;
    [[nodiscard]] std::optional<Entity> parent(Entity child) const;

    bool setParent(EntityId childId, EntityId newParentId);
    void detachFromParent(EntityId childId);
    [[nodiscard]] EntityId getParent(EntityId entityId) const;
    [[nodiscard]] std::vector<EntityId> getChildren(EntityId entityId) const;
    [[nodiscard]] std::vector<EntityId> getRootEntities() const;
    [[nodiscard]] bool isDescendantOf(EntityId entityId, EntityId potentialAncestorId) const;

    void clear();

private:
    template <typename T>
    std::unordered_map<EntityId, T>& getOrCreateComponentPool() {
        auto it = m_componentPools.find(typeid(T));
        if (it == m_componentPools.end()) {
            it = m_componentPools.emplace(typeid(T), std::unordered_map<EntityId, T>{}).first;
        }
        return std::any_cast<std::unordered_map<EntityId, T>&>(it->second);
    }

    EntityId m_nextEntityId{1};
    std::vector<EntityId> m_entities;
    std::unordered_map<EntityId, bool> m_alive;
    std::unordered_map<core::GUID, EntityId> m_guidToEntity;
    std::unordered_map<std::type_index, std::any> m_componentPools;
};

class Entity {
public:
    Entity() = default;
    Entity(EntityId id, World* world) : m_id(id), m_world(world) {}

    [[nodiscard]] bool isValid() const noexcept { return m_world && m_world->isAlive(m_id); }
    [[nodiscard]] EntityId getId() const noexcept { return m_id; }
    [[nodiscard]] EntityId id() const noexcept { return m_id; }
    [[nodiscard]] core::GUID getGUID() const;
    [[nodiscard]] std::string_view getName() const;

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        return m_world->addComponent<T>(m_id, std::forward<Args>(args)...);
    }

    template <typename T>
    [[nodiscard]] T* getComponent() {
        return m_world->getComponent<T>(m_id);
    }

    template <typename T>
    [[nodiscard]] const T* getComponent() const {
        return m_world->getComponent<T>(m_id);
    }

    template <typename T>
    [[nodiscard]] bool hasComponent() const noexcept {
        return m_world->hasComponent<T>(m_id);
    }

    template <typename T>
    void removeComponent() {
        m_world->removeComponent<T>(m_id);
    }

    template <typename T, typename... Args>
    T& add(Args&&... args) {
        return addComponent<T>(std::forward<Args>(args)...);
    }

    void destroy() {
        if (isValid()) {
            m_world->destroyEntity(m_id);
        }
    }

    bool operator==(const Entity& rhs) const noexcept { return m_id == rhs.m_id && m_world == rhs.m_world; }
    bool operator!=(const Entity& rhs) const noexcept { return !(*this == rhs); }

private:
    EntityId m_id{kInvalidEntityId};
    World* m_world{nullptr};
};

template <typename T, typename... Args>
inline T& World::add(const Entity& entity, Args&&... args) {
    return addComponent<T>(entity.getId(), std::forward<Args>(args)...);
}

template <typename T>
inline T* World::tryGet(const Entity& entity) {
    return entity.isValid() ? getComponent<T>(entity.getId()) : nullptr;
}

template <typename T>
inline const T* World::tryGet(const Entity& entity) const {
    return entity.isValid() ? getComponent<T>(entity.getId()) : nullptr;
}

} // namespace engine::ecs

namespace std {
template <>
struct hash<engine::ecs::Entity> {
    std::size_t operator()(const engine::ecs::Entity& entity) const noexcept {
        return std::hash<engine::ecs::EntityId>{}(entity.getId());
    }
};
} // namespace std

namespace engine {
    using ecs::World;
    using ecs::Entity;
    using ecs::IDComponent;
    using ecs::TagComponent;
    using ecs::TransformComponent;
    using ecs::SpriteComponent;
    using ecs::MovementComponent;
    using ecs::HierarchyComponent;
} // namespace engine

