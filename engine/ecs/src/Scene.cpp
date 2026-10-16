#include "engine/ecs/Scene.hpp"

namespace engine::ecs {

Scene::Scene(std::string_view name)
    : m_name(name)
    , m_guid(core::GUID::generate()) {}

Scene::~Scene() {
    clear();
}

Entity Scene::createEntity(std::string_view name, const core::GUID& guid) {
    return m_world.createEntity(name, guid);
}

void Scene::destroyEntity(Entity entity) {
    if (entity.isValid()) {
        m_world.destroyEntity(entity.getId());
    }
}

void Scene::clear() {
    m_world.clear();
}

} // namespace engine::ecs
