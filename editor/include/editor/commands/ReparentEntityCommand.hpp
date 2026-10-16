#pragma once

#include "editor/UndoManager.hpp"
#include "engine/ecs/World.hpp"
#include <optional>

namespace editor {

class ReparentEntityCommand final : public UndoCommand {
public:
    ReparentEntityCommand(
        engine::World& world,
        engine::Entity child,
        engine::Entity newParent
    )
        : world_(world), child_(child), newParent_(newParent) {
        if (child.isValid()) {
            oldParent_ = world_.parent(child);
        }
    }

    void undo() override {
        if (child_.isValid()) {
            if (oldParent_.has_value() && oldParent_->isValid()) {
                world_.setParent(child_, *oldParent_);
            } else {
                world_.clearParent(child_);
            }
        }
    }

    void redo() override {
        if (child_.isValid() && newParent_.isValid()) {
            world_.setParent(child_, newParent_);
        }
    }

    [[nodiscard]] std::string description() const override {
        return "Reparent entity";
    }

private:
    engine::World& world_;
    engine::Entity child_;
    engine::Entity newParent_;
    std::optional<engine::Entity> oldParent_;
};

} // namespace editor
