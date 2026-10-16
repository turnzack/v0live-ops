#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace editor {

class UndoCommand {
public:
    virtual ~UndoCommand() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
    [[nodiscard]] virtual std::string description() const = 0;
};

class UndoManager {
public:
    void execute(std::unique_ptr<UndoCommand> command);
    bool undo();
    bool redo();
    void clear();

    [[nodiscard]] bool canUndo() const noexcept;
    [[nodiscard]] bool canRedo() const noexcept;
    [[nodiscard]] std::size_t historySize() const noexcept;

private:
    std::vector<std::unique_ptr<UndoCommand>> undoStack_;
    std::vector<std::unique_ptr<UndoCommand>> redoStack_;
};

} // namespace editor
