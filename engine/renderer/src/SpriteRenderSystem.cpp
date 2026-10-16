#include "engine/renderer/SpriteRenderSystem.hpp"

namespace engine {

void SpriteRenderSystem::update(ecs::World& world, SpriteBatch& batch) {
    batch.begin();

    world.forEach<ecs::TransformComponent, ecs::SpriteComponent>(
        [&](ecs::EntityId, ecs::TransformComponent& transform, ecs::SpriteComponent& sprite) {
            batch.draw({
                .position = glm::vec2(transform.position.x, transform.position.y),
                .size = glm::vec2(sprite.size.x, sprite.size.y),
                .rotation = transform.rotation.z,
                .color = glm::vec4(sprite.colorTint.x, sprite.colorTint.y, sprite.colorTint.z, sprite.colorTint.w)
            });
        }
    );

    batch.end();
}

} // namespace engine
