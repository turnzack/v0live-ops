#include "editor/UndoManager.hpp"
#include <spdlog/spdlog.h>

namespace editor {

void UndoManager::execute(std::unique_ptr<UndoCommand> command) {
    if (command == nullptr) {
        return;
    }

    command->redo();
    spdlog::debug("Undo command executed: {}", command->description());

    redoStack_.clear();
    undoStack_.push_back(std::move(command));
}

bool UndoManager::undo() {
    if (undoStack_.empty()) {
        return false;
    }

    auto command = std::move(undoStack_.back());
    undoStack_.pop_back();

    command->undo();
    redoStack_.push_back(std::move(command));

    return true;
}

bool UndoManager::redo() {
    if (redoStack_.empty()) {
        return false;
    }

    auto command = std::move(redoStack_.back());
    redoStack_.pop_back();

    command->redo();
    undoStack_.push_back(std::move(command));

    return true;
}

void UndoManager::clear() {
    undoStack_.clear();
    redoStack_.clear();
}

bool UndoManager::canUndo() const noexcept {
    return !undoStack_.empty();
}

bool UndoManager::canRedo() const noexcept {
    return !redoStack_.empty();
}

std::size_t UndoManager::historySize() const noexcept {
    return undoStack_.size();
}

} // namespace editor
