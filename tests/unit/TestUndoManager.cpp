#include <doctest/doctest.h>
#include "editor/UndoManager.hpp"

using namespace editor;

namespace {

class CounterCommand final : public UndoCommand {
public:
    explicit CounterCommand(int& value) : value_(value) {}

    void undo() override {
        --value_;
    }

    void redo() override {
        ++value_;
    }

    [[nodiscard]] std::string description() const override {
        return "Increment counter";
    }

private:
    int& value_;
};

} // namespace

TEST_CASE("UndoManager executes, undoes and redoes commands") {
    int value = 0;
    UndoManager undo;

    undo.execute(std::make_unique<CounterCommand>(value));
    undo.execute(std::make_unique<CounterCommand>(value));

    CHECK(value == 2);
    CHECK(undo.canUndo());

    CHECK(undo.undo());
    CHECK(value == 1);

    CHECK(undo.undo());
    CHECK(value == 0);

    CHECK_FALSE(undo.canUndo());

    CHECK(undo.redo());
    CHECK(value == 1);

    CHECK(undo.redo());
    CHECK(value == 2);

    CHECK_FALSE(undo.canRedo());
}

TEST_CASE("Executing a new command clears the redo stack") {
    int value = 0;
    UndoManager undo;

    undo.execute(std::make_unique<CounterCommand>(value));
    undo.undo();

    CHECK(undo.canRedo());

    undo.execute(std::make_unique<CounterCommand>(value));

    CHECK_FALSE(undo.canRedo());
    CHECK(value == 1);
}
