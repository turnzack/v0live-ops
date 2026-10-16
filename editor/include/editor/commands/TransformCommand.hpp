#pragma once

#include <glm/glm.hpp>
#include "editor/UndoManager.hpp"
#include "engine/ecs/World.hpp"

namespace editor {

class TransformCommand final : public UndoCommand {
public:
    TransformCommand(
        engine::World& world,
        engine::Entity entity,
        glm::vec3 previousPosition,
        glm::vec3 newPosition
    )
        : world_(world),
          entity_(entity),
          previousPosition_(previousPosition),
          newPosition_(newPosition) {
    }

    void undo() override {
        if (auto* transform = world_.tryGet<engine::TransformComponent>(entity_)) {
            transform->position = {previousPosition_.x, previousPosition_.y, previousPosition_.z};
        }
    }

    void redo() override {
        if (auto* transform = world_.tryGet<engine::TransformComponent>(entity_)) {
            transform->position = {newPosition_.x, newPosition_.y, newPosition_.z};
        }
    }

    [[nodiscard]] std::string description() const override {
        return "Move entity";
    }

private:
    engine::World& world_;
    engine::Entity entity_;
    glm::vec3 previousPosition_;
    glm::vec3 newPosition_;
};

} // namespace editor
