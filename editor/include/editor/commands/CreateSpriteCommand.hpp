#pragma once

#include "editor/UndoManager.hpp"
#include "engine/ecs/World.hpp"
#include "engine/ecs/Components.hpp"
#include <glm/glm.hpp>
#include <string>

namespace editor {

class CreateSpriteCommand final : public UndoCommand {
public:
    CreateSpriteCommand(
        engine::World& world,
        std::string assetGuid,
        glm::vec2 spawnPosition
    )
        : world_(world), assetGuid_(std::move(assetGuid)), position_(spawnPosition) {}

    void undo() override {
        if (entity_.isValid()) {
            world_.destroyEntity(entity_.getId());
            entity_ = engine::Entity();
        }
    }

    void redo() override {
        entity_ = world_.createEntity("Sprite");
        auto& t = entity_.addComponent<engine::TransformComponent>();
        t.position = {position_.x, position_.y, 0.0f};
        auto& s = entity_.addComponent<engine::SpriteComponent>();
        s.textureGuid = engine::core::GUID(assetGuid_);
    }

    [[nodiscard]] std::string description() const override {
        return "Create sprite from asset " + assetGuid_;
    }

private:
    engine::World& world_;
    std::string assetGuid_;
    glm::vec2 position_;
    engine::Entity entity_;
};

} // namespace editor
