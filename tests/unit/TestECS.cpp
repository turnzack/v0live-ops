#include "engine/ecs/World.hpp"
#include "engine/ecs/SystemScheduler.hpp"
#include <iostream>
#include <string_view>
#include <functional>

namespace test {
    void registerTest(std::string_view name, std::function<bool()> testFunc);
}

#define TEST_CASE(name) \
    static bool test_func_##name(); \
    static const bool test_registered_##name = []() { \
        ::test::registerTest(#name, test_func_##name); \
        return true; \
    }(); \
    static bool test_func_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return false; \
        } \
    } while (false)

using namespace engine::ecs;

TEST_CASE(ECS_EntityCreationAndComponents) {
    World world;
    CHECK(world.entityCount() == 0);

    Entity player = world.createEntity("Player");
    CHECK(player.isValid());
    CHECK(player.getName() == "Player");
    CHECK(player.getGUID().isValid());
    CHECK(world.entityCount() == 1);

    // Transform est ajouté par défaut
    auto* t = player.getComponent<TransformComponent>();
    CHECK(t != nullptr);
    t->position = {10.0f, 20.0f, 0.0f};

    // Ajout d'un SpriteComponent
    auto& s = player.addComponent<SpriteComponent>(engine::core::Vec2(64.0f, 64.0f));
    s.sortingLayer = 2;
    CHECK(player.hasComponent<SpriteComponent>());
    CHECK(s.size.x == 64.0f && s.size.y == 64.0f);

    // Ajout d'un MovementComponent
    auto& m = player.addComponent<MovementComponent>(300.0f);
    CHECK(player.hasComponent<MovementComponent>());
    CHECK(m.speed == 300.0f);

    // Récupération par GUID
    Entity retrieved = world.getEntityByGUID(player.getGUID());
    CHECK(retrieved.isValid());
    CHECK(retrieved.getId() == player.getId());

    return true;
}

TEST_CASE(ECS_SystemPhasesExecution) {
    World world;
    SystemScheduler scheduler;

    Entity e = world.createEntity("MovingEntity");
    auto* t = e.getComponent<TransformComponent>();
    t->position = {0.0f, 0.0f, 0.0f};
    e.addComponent<MovementComponent>(100.0f);

    bool preUpdateRun = false;
    bool updateRun = false;
    bool fixedUpdateRun = false;

    scheduler.registerSystem(SystemPhase::PreUpdate, "InputSystem", [&](World&, float) {
        preUpdateRun = true;
    });

    scheduler.registerSystem(SystemPhase::FixedUpdate, "MovementSystem", [&](World& w, float dt) {
        fixedUpdateRun = true;
        w.forEach<TransformComponent, MovementComponent>([&](EntityId, TransformComponent& trans, MovementComponent& mov) {
            trans.position.x += mov.speed * dt;
        });
    });

    scheduler.registerSystem(SystemPhase::Update, "GameplaySystem", [&](World&, float) {
        updateRun = true;
    });

    // Exécution séquentielle des phases
    scheduler.runPhase(SystemPhase::PreUpdate, world, 0.016f);
    CHECK(preUpdateRun);

    scheduler.runPhase(SystemPhase::FixedUpdate, world, 0.016f);
    CHECK(fixedUpdateRun);
    CHECK(t->position.x > 0.0f); // Le mouvement a bien été appliqué

    scheduler.runPhase(SystemPhase::Update, world, 0.016f);
    CHECK(updateRun);

    return true;
}
