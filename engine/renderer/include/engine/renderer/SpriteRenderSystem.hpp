#pragma once

#include "engine/ecs/World.hpp"
#include "engine/renderer/SpriteBatch.hpp"

namespace engine {

class SpriteRenderSystem {
public:
    void update(ecs::World& world, SpriteBatch& batch);
};

} // namespace engine
