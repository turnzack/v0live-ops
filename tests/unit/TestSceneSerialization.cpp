#include "engine/ecs/Scene.hpp"
#include "engine/ecs/SceneSerializer.hpp"
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

TEST_CASE(Scene_RoundTripJsonSerialization) {
    Scene originalScene("Level_01_Platformer");
    auto originalSceneGuid = originalScene.getGUID();

    // 1. Entite Joueur
    Entity player = originalScene.createEntity("Player");
    auto playerGuid = player.getGUID();
    auto* playerTransform = player.getComponent<TransformComponent>();
    playerTransform->position = {150.0f, 300.0f, 0.0f};
    playerTransform->scale = {2.0f, 2.0f, 1.0f};

    auto& playerSprite = player.addComponent<SpriteComponent>(engine::core::Vec2(32.0f, 48.0f));
    playerSprite.sortingLayer = 5;
    playerSprite.flipX = false;

    auto& playerMov = player.addComponent<MovementComponent>(250.0f);
    playerMov.velocity = {50.0f, 0.0f};

    // 2. Entite Plateforme
    Entity platform = originalScene.createEntity("Ground_Platform");
    auto platformGuid = platform.getGUID();
    auto* platformTransform = platform.getComponent<TransformComponent>();
    platformTransform->position = {0.0f, 500.0f, 0.0f};
    platformTransform->scale = {10.0f, 1.0f, 1.0f};

    auto& platformSprite = platform.addComponent<SpriteComponent>(engine::core::Vec2(100.0f, 20.0f));
    platformSprite.sortingLayer = 1;

    // Premier export JSON
    SceneSerializer serializer1(originalScene);
    const std::string jsonOutput1 = serializer1.serializeToString();

    CHECK(jsonOutput1.find("\"sceneName\": \"Level_01_Platformer\"") != std::string::npos);
    CHECK(jsonOutput1.find(playerGuid.asString()) != std::string::npos);
    CHECK(jsonOutput1.find(platformGuid.asString()) != std::string::npos);
    CHECK(jsonOutput1.find("\"speed\": 250.0000") != std::string::npos);

    // Rechargement dans une nouvelle scene
    Scene loadedScene;
    SceneSerializer deserializer(loadedScene);
    bool ok = deserializer.deserializeFromString(jsonOutput1);
    CHECK(ok);

    CHECK(loadedScene.getName() == "Level_01_Platformer");
    CHECK(loadedScene.getGUID() == originalSceneGuid);
    CHECK(loadedScene.getWorld().entityCount() == 2);

    // Verification du joueur recharge par GUID
    Entity loadedPlayer = loadedScene.getWorld().getEntityByGUID(playerGuid);
    CHECK(loadedPlayer.isValid());
    CHECK(loadedPlayer.getName() == "Player");

    auto* loadedPlayerTrans = loadedPlayer.getComponent<TransformComponent>();
    CHECK(loadedPlayerTrans != nullptr);
    CHECK(std::abs(loadedPlayerTrans->position.x - 150.0f) < 1e-3f);
    CHECK(std::abs(loadedPlayerTrans->position.y - 300.0f) < 1e-3f);
    CHECK(std::abs(loadedPlayerTrans->scale.x - 2.0f) < 1e-3f);

    auto* loadedPlayerSprite = loadedPlayer.getComponent<SpriteComponent>();
    CHECK(loadedPlayerSprite != nullptr);
    CHECK(std::abs(loadedPlayerSprite->size.x - 32.0f) < 1e-3f);
    CHECK(std::abs(loadedPlayerSprite->size.y - 48.0f) < 1e-3f);
    CHECK(loadedPlayerSprite->sortingLayer == 5);

    auto* loadedPlayerMov = loadedPlayer.getComponent<MovementComponent>();
    CHECK(loadedPlayerMov != nullptr);
    CHECK(std::abs(loadedPlayerMov->speed - 250.0f) < 1e-3f);

    // Verification de la plateforme
    Entity loadedPlatform = loadedScene.getWorld().getEntityByGUID(platformGuid);
    CHECK(loadedPlatform.isValid());
    CHECK(loadedPlatform.getName() == "Ground_Platform");

    // Second export JSON : Round-trip exact
    SceneSerializer serializer2(loadedScene);
    const std::string jsonOutput2 = serializer2.serializeToString();
    CHECK(jsonOutput1 == jsonOutput2);

    return true;
}
