#include <doctest/doctest.h>
#include "engine/ecs/World.hpp"
#include "editor/UndoManager.hpp"
#include "editor/commands/ReparentEntityCommand.hpp"

using namespace engine;
using namespace editor;

TEST_CASE("Hierarchy parent/child relationship and cycle prevention") {
    ecs::World world;

    ecs::Entity parent = world.createEntity("Parent");
    ecs::Entity child = world.createEntity("Child");
    ecs::Entity grandChild = world.createEntity("GrandChild");

    // Initially all roots
    auto roots = world.getRootEntities();
    CHECK(roots.size() == 3);

    // Set parent
    world.setParent(child, parent);
    CHECK(world.parent(child).has_value());
    CHECK(world.parent(child).value() == parent);

    auto pChildren = world.children(parent);
    CHECK(pChildren.size() == 1);
    CHECK(pChildren[0] == child);

    roots = world.getRootEntities();
    CHECK(roots.size() == 2);

    // Set grandchild
    world.setParent(grandChild, child);
    CHECK(world.parent(grandChild).value() == child);
    roots = world.getRootEntities();
    CHECK(roots.size() == 1);
    CHECK(roots[0] == parent.getId());

    // Cycle prevention: cannot set parent's parent to grandChild
    bool cycleSet = world.setParent(parent.getId(), grandChild.getId());
    CHECK_FALSE(cycleSet);

    // Clear parent
    world.clearParent(grandChild);
    CHECK_FALSE(world.parent(grandChild).has_value());
    roots = world.getRootEntities();
    CHECK(roots.size() == 2);
}

TEST_CASE("ReparentEntityCommand with Undo and Redo") {
    ecs::World world;
    UndoManager undo;

    ecs::Entity parentA = world.createEntity("ParentA");
    ecs::Entity parentB = world.createEntity("ParentB");
    ecs::Entity item = world.createEntity("Item");

    world.setParent(item, parentA);
    CHECK(world.parent(item).value() == parentA);

    // Reparent item to parentB
    undo.execute(std::make_unique<ReparentEntityCommand>(world, item, parentB));
    CHECK(world.parent(item).value() == parentB);
    CHECK(world.children(parentB).size() == 1);
    CHECK(world.children(parentA).empty());

    // Undo -> item should be back under parentA
    undo.undo();
    CHECK(world.parent(item).value() == parentA);
    CHECK(world.children(parentA).size() == 1);
    CHECK(world.children(parentB).empty());

    // Redo -> item should be back under parentB
    undo.redo();
    CHECK(world.parent(item).value() == parentB);
    CHECK(world.children(parentB).size() == 1);
}
